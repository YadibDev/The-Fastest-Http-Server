#include "Request.hpp"

clsRequest::clsRequest()
    : _state(READING_LINE), _startOfHeader(true), _headerParser(_headers) {}
    

void	clsRequest::getDataParse(const std::string &RawData)
{
    if (RawData.empty())
        return ;

    if (_state == READING_BODY)
    {
        _Buffer += _Remainder + RawData;
        _Remainder = "";
        return ;
    }
    size_t pos = RawData.find("\r\n");

    if (pos != std::string::npos)
    {
        _Buffer = _Remainder + RawData.substr(0, pos + 2);
        _Remainder = RawData.substr(pos + 2);
    }
    else
        _Remainder = _Remainder + RawData;
}

void clsRequest::parse(const std::string& rawData)
{
    getDataParse(rawData);
    if (_state == READING_LINE)
    {
        size_t pos = _Buffer.find("\r\n");
        if (pos != std::string::npos)
        {
            _startLine = clsStartLine(_Buffer.substr(0, pos));
            _Buffer = _Buffer.substr(pos + 2);
            _arguments._Data = _Buffer;
            _arguments._Pos = pos + 2;
            _state = READING_HEADERS;
        }
    }
    if (_state == READING_HEADERS)
    {
        _headerParser.parseHeader(_arguments, _startOfHeader);
    }
    if (_state == READING_BODY)
    {
        // we can read the body
        _state = COMPLETED;
    }
}
    
bool clsRequest::isCompleted() const { return _state == COMPLETED; }