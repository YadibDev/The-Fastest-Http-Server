#pragma once
#include <string>
#include <vector>
#include <map>
#include <stack>


enum Method
{
    GET = 1 << 0,
    POST = 1 << 1,
    DELETE = 1 << 2
};

class RequestHandler
{
public:

    std::string physical_path;
    bool autoindex;
    int allow_methods;
    Method method;
private:

    std::string _query;
    std::map<std::string, std::vector<std::string> > _headers;
    std::map<int, std::string> _error_pages;
    std::map<std::string, std::string>	_cgi_interpreter;
    std::string _body;
    std::string _filePathBody;
public:

    int return_status;
    std::string return_url;

    std::string upload_store;


    RequestHandler()
        : physical_path(""),
          autoindex(false),
          allow_methods(0),
          method(GET),
          return_status(0),
          return_url(""),
          upload_store("")
    {
    }
};
