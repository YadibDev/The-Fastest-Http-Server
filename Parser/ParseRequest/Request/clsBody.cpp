#include "clsBody.hpp"
#include "Request.hpp"

#define MAX_BODY_RAM 20000
// geters
const bool &clsBody::getIsError() const
{
    return _isError;
}
const std::string &clsBody::getFileName() const
{
    return _fileName;
}
const std::string &clsBody::getBodyInRam() const
{
    return _bodyBuffer;
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
    this->_bodyBuffer = "";
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
    if (_state == SETTING_VARS || _state == DONE_WIHTERROR || _state == DONE_GOOD)
    {
        this->Reset();
        if (req._headers.count("Content-length"))
        {
            _isChunk = false;
            const char *content_leng = req._headers["Content-Length"][0].c_str(); // i will change this
            _Length = std::atol(content_leng);
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
                _bodyBuffer.resize(_Length);
            }
            _state = bodySteps::READING_BODY;
        }
        // add chunked
        // aftter finish chunked add multipart
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
                // if (multipart add algo)
            // else
                std::cout << arr[t++];
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
        /*
        if (contentLength)
            if (not multipart)
                nBytes = write(this->fd, buffer, nBytes); // i will change this
            else if (multipart)
                algoMultipart

        if (chunk)
            _handleChunk
        
        
        if (nBytes == -1)
        {
            this->_isError = true;
            return ;
        }
        */
    }
    else if (_bodyLocation == RAM)
    {
        /*
            if (multipart)
                algo dyal multipart
            else
                check is hit content_length  
                _state = DONE;
        */
    }

    if (_state == DONE_GOOD || _state == DONE_WIHTERROR)
    {
        if (fd != -1)
            close (fd);
        fd = -1;
    }
}
