#ifndef HEADER_HPP
#define HEADER_HPP

#include <map>
#include <string>
#include "ParseRequest/Request.hpp"  // for stArguments and RequestState

class ParseHeader {
public:
	// constructor takes a reference to a map that will be populated
	ParseHeader(std::map<std::string, std::string>& headers);

	// parse headers from args starting at args._Pos; TheStartOfHeader guards
	// against leading whitespace when called for the first header line.
	bool parseHeader(stArguments args, bool &TheStartOfHeader);

private:
	RequestState state;
	std::map<std::string, std::string> &headerMap;

	bool	checkHeaderField(std::string HeaderField);
	bool	getValue(stArguments args, std::string &fieldValue);
};

#endif
