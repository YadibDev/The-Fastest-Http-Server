#include "clsBody.hpp"
#include "Request.hpp"

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

const bodyPlace &clsBody::getBodyLocation() const
{
    return _bodyLocation;
}

const bodySteps &clsBody::getState() const
{
    return _state;
}
// mehtods
void clsBody::Reset()
{
    this->_fileName = "tmp/file_XXXXXX";
    this->_bodyLocation = NONE;
    this->_isError = false;
    this->_state = SETTING_VARS;
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
void clsBody::bodyHandler(const std::string &buffer, clsRequest &req)
{
    // i must handle left data in `request meta data` case
    if (_state == SETTING_VARS || _state == DONE_WIHTERROR || _state == DONE_GOOD)
    {
        this->Reset();
        if (data.known_headers[HttpTables::H_TRANSFER_ENCODING].hash != -1 && data.known_headers[HttpTables::H_TRANSFER_ENCODING].value == "chunked")
        {
            _isChunk = true;
            _bodyLocation = bodyPlace::DISK;
            fd = mkstemp(&_fileName[0]);
            if (fd == -1)
            {
                this->_isError = true;
                return;
            }
        }
        else if (data.known_headers[HttpTables::Content-length].hash != -1)
        {
            _isChunk = false;
            const char *content_leng = data.known_headers[HttpTables::Content-length].value; //
            _Length = std::atol(content_leng); // maybe handle overflow and add check if he  is more than the limit in config fie
            if (_Length > MAX_BODY_RAM)
            {
                _bodyLocation = bodyPlace::DISK;
                fd = mkstemp(&_fileName[0]);
                if (fd == -1)
                {
                    this->_isError = true;
                    return;
                }
            }
            else
            {
                _bodyLocation = bodyPlace::RAM;
            }
            _state = bodySteps::READING_BODY;
        }
        
        if (data.known_headers[HttpTables::H_CONTENT_TYPE].hash != -1 && data.known_headers[HttpTables::H_CONTENT_TYPE].value == "multipart")
            _isMultiPart = true;
        else
            _isMultiPart = false;
    }
}

void clsBody::_handleChunk(size_t ofset)
{
    // pointing to data
    char *arr = data.io_chunk;
    size_t &cur = chunkHelp.cur;
    size_t &t = chunkHelp.trav;
    bool &readSize = chunkHelp.readsize;
    size_t &size = chunkHelp.size;
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
                        _state = DONE_GOOD;
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
            while (t < ofset && (t - cur) < size)
            {
            //     if (_isMultiPart)
            //         algoMultipart
            //     else if (multipart)
            //         nBytes = write(this->fd, buffer, nBytes); // i will change this
            }
            if (t + 1 < ofset && t - cur == size)
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
            _state = DONE_WIHTERROR;
            return ;
        }
    }
}

void clsBody::normalBody(const char *buffer, ssize_t offset)
{
    if (_bodyLocation == DISK)
    {
        // 5asni ndir b7sab dik l3ayba dyal body ba9i f meta request
        if (_isChunk == false)
        {
            if (data.known_headers[H_CONTENT_TYPE].hash != -1)
            {
                if (_isMultiPart)
                {
                    _multipartLib.Parser(data.io_chunk, offset);
                    if (_multipartLib.getError())
                        _state = bodySteps::DONE_WIHTERROR;
                }
                else
                    nBytes = write(this->fd, buffer, nBytes); // i will change this
            }
        }
        else
            _handleChunk(offset)
        if (nBytes == -1)
        {
            this->_isError = true;
            return ;
        }
    }
    else if (_bodyLocation == RAM)
    {

            if (offset == _Length)
            {
                if (_isMultiPart)
                {
                    _multipartLib.Parser(data.io_chunk, offset);
                    if (_multipartLib.getError())
                        _state = bodySteps::DONE_WIHTERROR;
                    else
                        _state = bodySteps::DONE_GOOD;
                }
                _state = bodySteps::DONE_GOOD;
            }
    }

    if (_state == DONE_GOOD || _state == DONE_WIHTERROR)
    {
        if (fd != -1)
            close (fd);
        fd = -1;
    }
}
