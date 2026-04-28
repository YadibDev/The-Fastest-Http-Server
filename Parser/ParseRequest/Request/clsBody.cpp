#include "clsBody.hpp"

#define MAX_BODY_RAM 8100

// geters

#define DEFAULT_TEMP "/goinfre/yadib/The-Fastest-Http-Server/temp/fileXXXXXX"

clsBody::clsBody(stPollRequest &p) : data(p)
{
    fd = -1;
    this->_fileName = DEFAULT_TEMP; // should be /tmp insted of this path
    this->_state = clsBody::SETTING_VARS;
    this->_isChunk = false;
    this->_contentLength = 0;
    writeSize = 0;
    chunkHelp.Reset();
    _errorPage.setStatus(0);
}

const std::string &clsBody::getFileName() const
{
    return _fileName;
}

clsBody::step clsBody::getState() const
{
    return _state;
}

// mehtods
void clsBody::Reset()
{
    this->_fileName = DEFAULT_TEMP; // should be /tmp insted of this path
    this->_state = clsBody::SETTING_VARS;
    this->_isChunk = false;
    this->_contentLength = 0;
    writeSize = 0;
    chunkHelp.Reset();
    _errorPage.setStatus(0);
    if (fd != -1)
        close(fd);
    fd = -1;
}

// working on normal body without chunk
bool clsBody::bodyHandler(uint16_t *off, const size_t &maxBodySize)
{
    uint16_t &offset = *off;

    if (_state == clsBody::SETTING_VARS || _state == clsBody::DONE_WIHTERROR || _state == clsBody::DONE_GOOD)
    {
        this->Reset();

        if (data.known_headers[HttpTables::H_TRANSFER_ENCODING].Hash != -1)
            _isChunk = true;
        else if (data.known_headers[HttpTables::H_CONTENT_LENGTH].Hash != -1)
        {
            _isChunk = false;
            const char *content_leng_str = data.known_headers[HttpTables::H_CONTENT_LENGTH].val.Data;

            if (!HelperFunctions::ConvertStrToNum(content_leng_str, _contentLength) || _contentLength > (long)maxBodySize)
            {
                if (_contentLength > (long)maxBodySize)
                    _errorPage.setStatus(413, "Content Too Large\n");
                else
                    _errorPage.setStatus(400, "Bad Request");

                _state = clsBody::DONE_WIHTERROR;
                return false;
            }
        }

        fd = mkstemp(&_fileName[0]);
        if (fd == -1)
        {
            _errorPage.setStatus(500, "Internal Server Error:");
            _state = clsBody::DONE_WIHTERROR;
            return false;
        }

        _state = clsBody::READING_BODY;
    }
    ParseBody(offset, maxBodySize); // i must change name of it
    return true;
}

void clsBody::readSizeChunk(uint16_t &ofset, bool &error)
{
    char *arr = data.io_chunk;
    uint16_t &cur = chunkHelp.cur;
    uint16_t &t = chunkHelp.trav;
    bool &readSize = chunkHelp.readsize;
    uint16_t &size = chunkHelp.size;

    if (arr[t] == '\r')
    {
        if (t + 1 < ofset && arr[t + 1] != '\n')
            error = true;
        else if (t + 1 < ofset && arr[t + 1] == '\n')
        {
            t += 2;
            char *end;
            size = strtol(&arr[cur], &end, 16);
            if (end[0] != '\r')
                error = true;
            else if (size == 0)
            {
                _state = clsBody::DONE_GOOD;
                return;
            }
            readSize = false;
            cur = t;
        }
    }
    else
        t++;
}

void clsBody::_handleChunk(uint16_t &ofset)
{
    // pointing to data
    char *arr = data.io_chunk;
    uint16_t &cur = chunkHelp.cur;
    uint16_t &t = chunkHelp.trav;
    bool &readSize = chunkHelp.readsize;
    uint16_t &size = chunkHelp.size;
    bool error = false;

    while (cur < ofset && t < ofset)
    {
        if (readSize)
            readSizeChunk(ofset, error);
        else
        {
            while (t < ofset && size)
            {
                int temp;

                if (ofset - t < size)
                    temp = write(this->fd, &arr[t], ofset - t);
                else
                    temp = write(this->fd, &arr[t], size);
                if (temp == -1)
                {
                    _errorPage.setStatus(500, "Internal Server Error:");
                    error = true;
                    break;
                }

                t += temp;
                size -= temp;
            }
            if (t + 1 < ofset && size <= 0)
            {
                if (arr[t] != '\r' || arr[t + 1] != '\n')
                {
                    _errorPage.setStatus(400, "Bad Request");
                    error = true;
                }
                else
                {
                    // add special case
                    t += 2;
                    cur = t;
                    readSize = true;
                }
            }
        }
        if (error)
        {
            _state = clsBody::DONE_WIHTERROR;
            return;
        }
    }
}

void clsBody::shiftingData(char *src, int offset, int sizeShift)
{
    for (int i = 0; i < sizeShift; i++)
    {
        src[i] = src[offset + i];
    }
}

void clsBody::StoreNormalBodyInDisk(uint16_t &offset)
{

    int temp = write(this->fd, data.io_chunk, offset); // i will change this
    if (temp == -1)
    {
        _errorPage.setStatus(500, "Internal Server Error:");
        _state = clsBody::DONE_WIHTERROR;
        return;
    }

    writeSize += temp;
    offset -= temp;

    if (offset > 0)
        shiftingData(data.io_chunk, temp, offset);

    if (this->_contentLength == writeSize)
        _state = clsBody::DONE_GOOD;
}

void clsBody::ParseBody(uint16_t &offset, const size_t &maxBodySize)
{
    (void)maxBodySize; // unused right now

    if (_isChunk == false)
    {
        StoreNormalBodyInDisk(offset);
    }
    else
        _handleChunk(offset);

    if (_state == clsBody::DONE_GOOD || _state == clsBody::DONE_WIHTERROR)
    {
        if (fd != -1)
            close(fd);
        fd = -1;
    }
}

ssize_t clsBody::getBodySize()
{
    return _contentLength;
}

HttpError clsBody::getError()
{
    return this->_errorPage;
}

clsBody::~clsBody()
{
    this->_fileName = DEFAULT_TEMP; // should be /tmp insted of this path
    this->_state = clsBody::SETTING_VARS;
    this->_isChunk = false;
    this->_contentLength = 0;
    writeSize = 0;
    chunkHelp.Reset();
    _errorPage.setStatus(0);
    if (fd != -1)
        close(fd);
    fd = -1;
}
