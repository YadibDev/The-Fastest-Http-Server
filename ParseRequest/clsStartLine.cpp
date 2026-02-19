#include "clsStartLine.hpp"
#include <sstream>

clsStartLine::clsStartLine() : _isAbsoluteURI(false), _method(GET), _scheme(), _host(), _port(), _path(), _query(), _version(), statuCode(200) {}

clsStartLine::clsStartLine(const std::string &startLine) {
	statuCode = 200;
	_parseStartLine(startLine);
}

bool clsStartLine::_parseMethod(const std::string &methodStr) {
	if (methodStr == "GET") 
		_method = GET;
	else if (methodStr == "POST")
		_method = POST;
	else if (methodStr == "DELETE")
		_method = DELETE;
	else {
		statuCode = 501;
		return false;
	}
	return true;
}

void clsStartLine::_parsURI(const std::string &URI_str) {
	URI	_uri(URI_str);

	_scheme = _uri.getScheme();
	_host   = _uri.getHost();
	{
		std::ostringstream oss;
		oss << _uri.getPort();
		_port = oss.str();
	}
	_path   = _uri.getPath();
	_query  = _uri.getQuery();
}

std::string clsStartLine::getPart(const std::string &str, size_t &indx) {
	HelperFunctions::skipWhitespace(str, indx);
	size_t nextSpace = str.find_first_of(" \t", indx);
	size_t len = (nextSpace == std::string::npos) ? str.size() - indx : (nextSpace - indx);
	std::string part = str.substr(indx, len);
	indx += len;
	return part;
}

bool clsStartLine::_parseVersion(const std::string &version) {
	if (version.length() != 8)
		return false;
	if (version == "HTTP/1.1" || version == "HTTP/1.0") {
		_version = version;
		return true;
	}
	statuCode = 505; // HTTP Version Not Supported
	return false;
}

void clsStartLine::_parseStartLine(std::string startLine) {
	if (startLine.empty()) {
		statuCode = 400;
		return;
	}
	startLine = HelperFunctions::trim(startLine);
	size_t indx = 0;
	_parseMethod(getPart(startLine, indx));
	_parsURI(getPart(startLine, indx));
	_parseVersion(getPart(startLine, indx));
	
	if (startLine.size() > 2)
		if (!HelperFunctions::isCRLF(startLine))
			statuCode = 400;
		
	
}

// Getters
bool               clsStartLine::isAbsoluteURI() const { return _isAbsoluteURI; }
eMethods           clsStartLine::getMethod() const { return _method; }
const std::string& clsStartLine::getHost()       const { return _host; }
const std::string& clsStartLine::getPath()       const { return _path; }
const std::string& clsStartLine::getQuery()      const { return _query; }
const std::string& clsStartLine::getVersion()    const { return _version; }
short              clsStartLine::getStatusCode() const { return statuCode; }