#include "Request.hpp"

clsRequest::clsRequest() : _state(READING_LINE) {}
    

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

void clsRequest::parse(const std::string& rawData);
{
    getDataParse(rawData);
}
    
bool clsRequest::isCompleted() const { return _state == COMPLETED; }