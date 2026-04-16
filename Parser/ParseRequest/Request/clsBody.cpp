#include "clsBody.hpp"


#define MAX_BODY_RAM 8100

// geters

clsBody::clsBody(stPollRequest &p) : data(p)
{
    fd = -1;
}

const bool &clsBody::getIsError() const
{
    return _isError;
}

const std::string &clsBody::getFileName() const
{
    return _fileName;
}

const char *clsBody::getBodyInRam() const
{
    return data.io_chunk;
}

clsBody::place clsBody::getBodyLocation() const
{
    return _bodyLocation;
}

clsBody::step clsBody::getState() const
{
    return _state;
}

// mehtods
void clsBody::Reset()
{
    this->_fileName = "tmp/file_XXXXXX";
    this->_bodyLocation = clsBody::NONE;
    this->_isError = false;
    this->_state = clsBody::SETTING_VARS;
    this->_isMultiPart = false;
    this->_isChunk = false;
    this->_Length = -1;
    chunkHelp.Reset();
    fd = -1;
}

bool clsBody::thereIsAline(const std::string &buffer, size_t &start, char c, char after)
{
    size_t len = buffer.size();
    if (len < start)
        return false;
    while (start < len && buffer[start] != c)
        start++;
    if (start < len - 1 && buffer[start] == c && buffer[start + 1] == after)
        return true;
    return false;
}
// working on normal body without chunk
void clsBody::bodyHandler(uint16_t *off)
{
    uint16_t &offset = *off;
    // i must handle left data in `request meta data` case
    if (_state == clsBody::SETTING_VARS || _state == clsBody::DONE_WIHTERROR || _state == clsBody::DONE_GOOD)
    {
        this->Reset();
        if (data.known_headers[HttpTables::H_TRANSFER_ENCODING].Hash != -1)
        {
            _isChunk = true;
            _bodyLocation = clsBody::DISK;
            fd = mkstemp(&_fileName[0]);
            if (fd == -1)
            {
                this->_isError = true;
                return;
            }
        }
        else if (data.known_headers[HttpTables::H_CONTENT_LENGTH].Hash != -1)
        {
            _isChunk = false;
            const char *content_leng = data.known_headers[HttpTables::H_CONTENT_LENGTH].val.Data; //
            _Length = std::atol(content_leng); // maybe handle overflow and add check if he  is more than the limit in config fie
            if (_Length > MAX_BODY_RAM)
            {
                _bodyLocation = clsBody::DISK;
                fd = mkstemp(&_fileName[0]);
                if (fd == -1)
                {
                    this->_isError = true ;
                    return;
                }
            }
            else
            {
                _bodyLocation = clsBody::RAM ;
            }
            _state = clsBody::READING_BODY ;
        }
        
        if ( data.known_headers[HttpTables::H_CONTENT_TYPE].Hash != -1 && data.known_headers[HttpTables::H_CONTENT_TYPE].val.len >= 9 && strncmp(data.known_headers[HttpTables::H_CONTENT_TYPE].val.Data, "multipart/", 9) == -1)
            _isMultiPart = true; 
        else
            _isMultiPart = false;
    }
    normalBody(offset); // i must change name of it
}

void clsBody::handleMultiChunk(uint16_t &t, uint16_t offset, uint16_t &size, char *io_chunk)
{
    uint16_t &len = chunkHelp.multiLength;
    char *arr = chunkHelp.chunkMulti;

    while (len < 8000 && t < offset && size)
    {
        arr[len++] = io_chunk[t++];
        size--;
    }

    if (size == 0 || t == offset || len == 8000)
    {
        _multipartLib.Parser(arr, len);

        uint16_t trv = _multipartLib.getTrav(); // index in multipart

        int i;
        for (i = 0; i < len - trv; i++)
        {
            arr[i] = arr[trv + 1];
        }
        len = i;
        _multipartLib.setTrav(0);
    }
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
        // read data size
        if (readSize)
        {
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
                        return ;
                    }
                    readSize = false;
                    cur = t;
                }
            }
            else
                t++;
        }
        // storing data 
        else
        {
            while (t < ofset && size)
            {
                if (_isMultiPart == false)
                {
                    int temp;

                    if (ofset - t < size)
                        temp = write(this->fd, &arr[t], ofset - t);
                    else
                        temp = write(this->fd, &arr[t], size);

                    if (temp == -1)
                    {
                        error = true;
                        break;
                    }
                    t += temp;
                    size -= temp;                    
                }
                else if (_isMultiPart)
                {
                    handleMultiChunk(t, ofset, size, arr);
                }
            }
            if (t + 1 < ofset && size <= 0)
            {
                if (arr[t] != '\r' || arr[t + 1] != '\n')
                    error = true;
                else
                {
                    std::cout << "\n---\n";
                    t += 2;
                    cur = t;
                    readSize = true;
                }
            }
        }
        if (error)
        {
            _state = clsBody::DONE_WIHTERROR;
            return ;
        }
    }
}
void clsBody::moveOffsetMulti(uint16_t &offset)
{
    uint16_t t = _multipartLib.getTrav();
    int i;
    for (i = 0; i < offset - t; i++)
    {
        data.io_chunk[i] = data.io_chunk[t + 1];
    }
    offset = i;
    _multipartLib.setTrav(0);
}

void clsBody::shiftingData(char *src, int offset, int sizeShift)
{
    for (int i = 0; i < sizeShift; i++)
    {
        src[i] = src[offset + i];
    }
}

void clsBody::normalBody(uint16_t &offset)
{
    static int writeSize = 0;
    if (_bodyLocation == clsBody::DISK)
    {
        // 5asni ndir b7sab dik l3ayba dyal body ba9i f meta request
        if (_isChunk == false)
        {
                if (_isMultiPart)
                {
                    _multipartLib.Parser(data.io_chunk, offset);
                    if (_multipartLib.getError())
                        _state = clsBody::DONE_WIHTERROR;
                    else if (offset == writeSize)
                    {
                        if (_multipartLib.hitEnd())
                            _state = clsBody::DONE_GOOD;
                        else
                            _state = clsBody::DONE_WIHTERROR;
                    }
                    else
                        moveOffsetMulti(offset);
                }
                else
                {
                    int temp = write(this->fd, data.io_chunk, offset); // i will change this 
                    if (temp == -1)
                    {
                        this->_isError = true;
                        return ;
                    }
                    writeSize += temp;
                    offset -= temp;

                    if (offset)
                    {
                        shiftingData(data.io_chunk, temp, offset);
                    }

                    if (this->_Length == writeSize)
                        _state = clsBody::DONE_GOOD;
                }
        }
        else
            _handleChunk(offset); // still note done it very well
    }
    else if (_bodyLocation == clsBody::RAM)
    {
            if (offset == _Length)
            {
                if (_isMultiPart)
                {
                    _multipartLib.Parser(data.io_chunk, offset);
                    if (_multipartLib.getError()) // check is multipart hit end
                        _state = clsBody::DONE_WIHTERROR;
                    else
                        _state = clsBody::DONE_GOOD;
                }
                _state = clsBody::DONE_GOOD;
            }
    }

    if (_state == clsBody::DONE_GOOD || _state == clsBody::DONE_WIHTERROR)
    {
        if (fd != -1)
            close (fd);
        fd = -1;
    }
}
