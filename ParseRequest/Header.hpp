#ifndef HEADER_HPP
#define HEADER_HPP

#include <map>
#include <string>
#include "HeaderRequest.hpp"  // for stArguments and RequestState

class ParseHeader {
public:
	// constructor takes a reference to a map that will be populated
	ParseHeader(std::map<std::string, std::vector<std::string> >& headers);
	
	bool 	parseSingleHeader(const std::string& line, HttpError& error);
	std::vector<std::string> getHeaderValues(const std::string& headerField) const;
	
	private:
	RequestState state;
	std::map<std::string, std::vector<std::string> > &headerMap;

	void	storeHeader(std::string &headerField, std::string &fieldValue, std::map<std::string, std::vector<std::string> > &headerMap);
	bool	checkDoubleCRLF(std::string &data, size_t pos);
	bool	getValue(stArguments &args, std::string &fieldValue);
	bool 	checkHeaderField(std::string &HeaderField);

};

#endif
