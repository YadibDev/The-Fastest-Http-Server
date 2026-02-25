#include "clsStartLine.hpp"
#include <sstream>

clsStartLine::clsStartLine() : _isAbsoluteURI(false), _method(Methods::GET), _scheme(), _host(), _port(), _path(), _query(), _version() {}

bool clsStartLine::_parseMethod(const std::string &methodStr) {

	if (methodStr.empty())
	{
		_error.setStatus(400, "Bad Request");
		return false;
	}

	if (methodStr == "GET") 
		_method = Methods::GET;
	else if (methodStr == "POST")
		_method = Methods::POST;
	else if (methodStr == "DELETE")
		_method = Methods::DELETE;
	else {
		_error.setStatus(501, "Not Implemented");
		return false;
	}
	return true;
}

void clsStartLine::_parsURI(const std::string &URI_str) {
    const size_t MAX_URI_LENGTH = 8192; 

    if (URI_str.empty())
    {
        _error.setStatus(400, "Bad Request");
        return ;
    }

    if (URI_str.length() > MAX_URI_LENGTH)
    {
        _error.setStatus(414, "Request-URI Too Long");
        return ;
    }

    URI _uri(URI_str);

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
	
	size_t nextSpace = str.find_first_of(" \t", indx);
	size_t len = (nextSpace == std::string::npos) ? str.size() - indx : (nextSpace - indx);
	std::string part = str.substr(indx, len);
	indx += len;
	HelperFunctions::skipWhitespace(str, indx);
	return part;
}

bool clsStartLine::_parseVersion(const std::string &version) {

	if (version.empty())
	{
		_error.setStatus(400, "Bad Request");
		return false;
	}

	if (version.length() != 8)
		return (_error.setStatus(505, "HTTP Version Not Supported"), false);
	if (version == "HTTP/1.1" || version == "HTTP/1.0")
	{
		_version = version;
		return true;
	}
	_error.setStatus(505, "HTTP Version Not Supported");
	return false;
}

void clsStartLine::parseStartLine(std::string startLine) {
	if (startLine.empty()) {
		_error.setStatus(400, "Bad Request");
		return;
	}
	startLine = HelperFunctions::trim(startLine);
	size_t indx = 0;
	_parseMethod(getPart(startLine, indx));
	_parsURI(getPart(startLine, indx));
	_parseVersion(getPart(startLine, indx));		
}

// Getters
bool				clsStartLine::isAbsoluteURI() const { return _isAbsoluteURI; }
Methods::eMethods	clsStartLine::getMethod() const { return _method; }
const std::string&	clsStartLine::getHost()       const { return _host; }
const std::string&	clsStartLine::getPath()       const { return _path; }
const std::string&	clsStartLine::getQuery()      const { return _query; }
const std::string&	clsStartLine::getVersion()    const { return _version; }
const HttpError&	clsStartLine::getError()	const { return _error; }
