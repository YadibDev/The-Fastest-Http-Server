#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>

#include "clsStartLine.hpp"
#include "Header.hpp"
#include "../../Header/HeaderFiles.hpp"



class clsRequest {
private:
	RequestStatus::e_state                        _state;
	std::map<std::string, std::vector<std::string> >  _headers;
	std::vector<char>                   _body;
	std::string							_Remainder;
	bool								_startOfHeader;
	HttpError							_error;
	
	void	getDataParse(const std::string &RawData);
	
public:
	std::string							_Buffer;
	clsStartLine					 	_startLine;
	ParseHeader							_headerParser;

	clsRequest();
	void parse(const std::string &rawData);
	bool isCompleted() const;
	const HttpError	&getError() const;
};

#endif