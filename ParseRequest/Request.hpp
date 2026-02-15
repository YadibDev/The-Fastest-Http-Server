#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include "../Utils/HttpError.hpp"
#include "../Utils/HelperFunctions.hpp"

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
	RequestState                        _state;
	std::string                         _method;
	std::string                         _uri;
	std::string                         _version;
	std::map<std::string, std::string>  _headers;
	std::vector<char>                   _body;
	stArguments 			   _arguments;

public:
	clsRequest();
	void parse(const std::string &rawData);
	bool isCompleted() const;
};

#endif