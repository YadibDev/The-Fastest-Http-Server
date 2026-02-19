#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include "HeaderRequest.hpp"
#include "clsStartLine.hpp"
#include "Header.hpp"



class clsRequest {
private:
	RequestState                        _state;
	std::string                         _uri;
	std::string                         _version;
	std::map<std::string, std::string>  _headers;
	std::vector<char>                   _body;
	stArguments							_arguments;
	std::string							_Buffer;
	std::string							_Remainder;

	void	getDataParse(const std::string &RawData);

public:
	clsStartLine					 	_startLine;
	ParseHeader							_headerParser;

	clsRequest();
	void parse(const std::string &rawData);
	bool isCompleted() const;
};

#endif