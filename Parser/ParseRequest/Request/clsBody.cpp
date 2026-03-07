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
    if (_state == SETTING_VARS || _state == DONE)
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
void clsBody::normalBody(const char *buffer, ssize_t nBytes)
{
    if (_bodyLocation == DISK)
    {
        nBytes = write(this->fd, buffer, nBytes); // i will change this
        if (nBytes == -1)
        {
            this->_isError = true;
            return;
        }
    }
    else if (_bodyLocation == RAM)
        _bodyBuffer.append(buffer, nBytes);

    this->_Length -= nBytes;
    if (this->_Length == 0)
    {
        if (fd != -1)
            close (fd);
        _state = DONE;
    }
    else if (this->_Length < 0)
        std::cout << "ERRRRROR body location in ram get more bytes than needed" << std::endl;
}
