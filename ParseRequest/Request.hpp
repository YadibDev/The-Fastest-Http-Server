#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>

class clsRequest {
private:
    std::string                         _method;
    std::string                         _uri;
    std::string                         _version;
    std::map<std::string, std::string>  _headers;
    std::vector<char>                   _body;
    RequestState                        _state;

public:
    Request() : _state(READING_LINE) {}
    
};

#endifش