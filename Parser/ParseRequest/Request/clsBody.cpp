#include "clsBody.hpp"

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
bool clsBody::bodyHandler(uint16_t *off, const size_t &maxBodySize, bool isCgi, const char *path)
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

        if (isCgi)
            fd = mkstemp(&_fileName[0]);
        else
            fd = open(path, O_CREAT | O_CLOEXEC | O_WRONLY | O_TRUNC, 0644); // i may change this

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

bool clsBody::readSizeChunk(uint16_t &ofset, bool &error, short &totRemoves)
{
    char *arr = data.io_chunk;
    uint16_t &cur = chunkHelp.cur;
    uint16_t &t = chunkHelp.trav;
    bool &readSize = chunkHelp.readsize;
    uint16_t &size = chunkHelp.size;

    if (arr[t] == '\r')
    {
        if (t + 1 < ofset && arr[t + 1] != '\n')
        {
            _errorPage.setStatus(400, "Bad Request");
            error = true;
            _state = clsBody::DONE_WIHTERROR;
        }
        else if (t + 1 < ofset && arr[t + 1] == '\n')
        {
            t += 2;
            totRemoves += t - cur;
            if (HelperFunctions::ConvertStrToNum(&arr[cur], size, 16) == false)
            {
                _errorPage.setStatus(400, "Bad Request");
                error = true;
                _state = clsBody::DONE_WIHTERROR;
            }
            else if (size == 0)
                _state = clsBody::DONE_GOOD;

            readSize = false;
            cur = t;
        }
        else
            return true;
    }
    else
    {
        t++;
    }
    return false;
}

bool clsBody::_saveChunkBody(uint16_t &ofset, bool &error, short &totRemoves)
{
    char *arr = data.io_chunk;
    uint16_t &cur = chunkHelp.cur;
    uint16_t &t = chunkHelp.trav;
    bool &readSize = chunkHelp.readsize;
    uint16_t &size = chunkHelp.size;

    int temp;
    if (ofset - t < size)
        temp = write(this->fd, &arr[t], ofset - t);
    else
        temp = write(this->fd, &arr[t], size);

    if (temp == -1)
    {
        _errorPage.setStatus(500, "Internal Server Error:");
        error = true;
        return false;  // error so we end the function
    }

    t += temp;
    totRemoves += temp;
    size -= temp;

    if (t + 1 < ofset && size <= 0)
    {
        if (arr[t] != '\r' || arr[t + 1] != '\n')
        {
            _errorPage.setStatus(400, "Bad Request");
            error = true;
            return false;  // error so we end the function
        }
        else
        {
            t += 2;
            totRemoves += 2;
            cur = t;
            readSize = true;
        }
    }
    else if (t + 1 >= ofset)
        return true;
    
    return false;
}
void clsBody::_handleChunk(uint16_t &ofset)
{
    // pointing to data
    char *arr = data.io_chunk;
    uint16_t &cur = chunkHelp.cur;
    uint16_t &t = chunkHelp.trav;
    bool &readSize = chunkHelp.readsize;
    bool error = false;
    short totRemoves = 0;

    std::cout << "stroe chunk chunk body\n"
              << std::endl;

    while (cur < ofset && t < ofset)
    {
        if (readSize)
        {
            if (readSizeChunk(ofset, error, totRemoves) == true)
                break;
        }
        else
        {
            if (_saveChunkBody(ofset, error, totRemoves))
                break;
        }
        if (error)
        {
            _state = clsBody::DONE_WIHTERROR;
            return;
        }
    }

    if (totRemoves != 0 && totRemoves < ofset)
        shiftingData(arr, totRemoves, (ofset - totRemoves));

    ofset -= totRemoves;
    cur = 0;
    t -= totRemoves;
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
    int toWrite = fmax(offset + writeSize, _contentLength);
    int temp = write(this->fd, data.io_chunk, toWrite); // i will change this
    if (temp == -1)
    {
        _errorPage.setStatus(500, "Internal Server Error:");
        _state = clsBody::DONE_WIHTERROR;
        return;
    }

    writeSize += temp;
    std::cout << "write size =====> " << writeSize << std::endl;
    std::cout << "_contentLength size =====> " << _contentLength << std::endl;
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
