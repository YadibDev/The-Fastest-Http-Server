#pragma once
#include <string>
#include <vector>
#include <map>
#include <stack>
#include "../ParseConfigFile/ServerConfig/ConfigDirectiveParser.hpp"


enum Method
{
	GET = 1 << 0,
	POST = 1 << 1,
	DELETE = 1 << 2
};

class RequestHandler
{
public:

private:

	std::string _physicalPath;
	bool        _autoindex;
	bool        _allowMethod;
	std::string _query;
	Method		_method;
	std::map<std::string, std::vector<std::string> > _headers;
	std::map<int, stErrorPagedata> _error_pages;
	std::map<std::string, std::string>	_cgiInterpreter;
	std::string _body;
	std::string _filePathBody;
public:

	int return_status;
	std::string return_url;

	std::string upload_store;


	RequestHandler()
		: _physicalPath(""),
		  _autoindex(false),
		  _allowMethod(0),
		  _method(GET),
		  return_status(0),
		  return_url(""),
		  upload_store("")
	{
	}
};
