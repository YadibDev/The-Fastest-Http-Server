#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include "../Utils/HttpError.hpp"

enum RequestState {
	READING_LINE,
	READING_HEADERS,
	READING_BODY,
	COMPLETED,
	ERROR
};

struct  stArguments {
	size_t		_Pos;
	HttpError	_Error;
	std::string	_Data;
};

class clsRequest {
private:
	std::string                         _method;
	std::string                         _uri;
	std::string                         _version;
	std::map<std::string, std::string>  _headers;
	std::vector<char>                   _body;
	stArguments 					   _arguments;

public:
	Request(std::string Data) : _state(READING_LINE), _arguments({0, HttpError(), Data}) {}
	
};

#endifش