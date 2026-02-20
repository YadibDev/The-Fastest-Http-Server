#include "Request.hpp"

clsRequest::clsRequest()
    : _state(READING_LINE), _startOfHeader(true), _headerParser(_headers) {}


void clsRequest::parse(const std::string& rawData)
{
    _Buffer += rawData;
    size_t pos;

    while ((pos = _Buffer.find("\r\n")) != std::string::npos)
    {
        std::string line = _Buffer.substr(0, pos);
        _Buffer.erase(0, pos + 2);

        if (_state == READING_LINE)
        {
            clsStartLine startLine(line);
            _startLine = startLine;
            _state = READING_HEADERS;
        }
        else if (_state == READING_HEADERS)
        {
            if (line.empty())
            {
                _state = READING_BODY;
                break;
            }
            if (!_headerParser.parseSingleHeader(line, _arguments._Error))
                return;
        }
        
        if (_state == READING_BODY)
            break;
    }

    if (_state == READING_BODY)
    {
        // Body
        _state = COMPLETED;
    }
}
bool clsRequest::isCompleted() const { return _state == COMPLETED; }