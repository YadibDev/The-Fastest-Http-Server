#ifndef HEADER_HPP
#define HEADER_HPP

#include "../../Header/HeaderFiles.hpp"

// class ParseHeader {
// public:
// 	// constructor takes a reference to a map that will be populated
// 	ParseHeader(std::map<std::string, std::vector<std::string> >& headers);
	
// 	bool 	parseSingleHeader(const std::string& line, HttpError& error);
// 	std::map<std::string, std::vector<std::string> > &getHeaderValues() const;
	
// 	private:
// 	RequestStatus::e_state state;
// 	std::map<std::string, std::vector<std::string> > &headerMap;

// 	void	storeHeader(std::string &headerField, std::string &fieldValue, std::map<std::string, std::vector<std::string> > &headerMap);
// 	bool	checkDoubleCRLF(std::string &data, size_t pos);
// 	bool	getValue(stArguments &args, std::string &fieldValue);
// 	bool 	checkHeaderField(std::string &HeaderField);

// };
















#include <string>
#include <map>
#include <cstdint>
#include <algorithm>
#include <iostream>


enum e_header_id {
    H_HOST = 0,
    H_CONTENT_LENGTH,
    H_CONTENT_TYPE,
    H_TRANSFER_ENCODING,
    H_CONNECTION,
    H_EXPECT,
    H_USER_AGENT,
    H_COOKIE,
    H_KNOWN_COUNT
};

class clsHeader {
private:
    std::string _known_headers[H_KNOWN_COUNT];
    
    uint32_t    _header_mask;
    
    std::map<std::string, std::string> _unknownHeaders;

    e_header_id _getHeaderID(const std::string& key) const;
    bool        _is_singleton(e_header_id id) const;
    std::string _parseKey(const std::string& line, size_t colonPos) const;
    std::string _parseValue(const std::string& line, size_t colonPos) const;
    void        _set_unknownHeader(const std::string& key, const std::string& value);

public:
    clsHeader();
    ~clsHeader();

    bool parseHeader(const std::string& line, HttpError& error);

    const std::string& getHeader(e_header_id id) const;
    const std::map<std::string, std::string>& getUnknownHeaders() const;
    uint32_t getHeaderMask() const;
    
    bool hasHeader(e_header_id id) const { return (_header_mask & (1 << id)); }
};


#endif
