#include "Request.hpp"

clsRequest::clsRequest() : _state(READING_LINE) {}
    

void	getDataParse(const std::string &RawData)
{
    if (RawData.empty())
        return ;
    size_t pos = RawData.find("\r\n");

    if (pos != std::string::npos)
    {
        _Buffer = _Remainder + RawData.substr(0, pos + 2);
        _Remainder = RawData.substr(pos + 2);
    }
    else
    {
        _Buffer = _Remainder + RawData;
        _Remainder = "";
    }
}

void clsRequest::parse(const std::string& rawData);
    
bool clsRequest::isCompleted() const { return _state == COMPLETED; }