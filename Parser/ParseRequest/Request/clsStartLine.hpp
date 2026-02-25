#ifndef CLSSTARTLINE_HPP
#define CLSSTARTLINE_HPP

#include "../../Header/HeaderFiles.hpp"


class clsStartLine {
public:

private:
	bool				_isAbsoluteURI;
	Methods::eMethods	_method;
	std::string			_scheme;
	std::string			_host;
	std::string			_port;
	std::string			_path;
	std::string			_query;
	std::string			_version;
	HttpError			_error;

	bool        _parseMethod(const std::string &methodStr);
	bool        _parseVersion(const std::string &version);
	void        _parsURI(const std::string &URI_str);
	std::string getPart(const std::string &str, size_t &indx);
	void        _parseStartLine(std::string startLine);

	// helper functions are provided by HelperFunctions; not class members

public:
	clsStartLine();
	clsStartLine(const std::string &startLine);
	
	// Getters
	bool				isAbsoluteURI() const;
	Methods::eMethods	getMethod()     const;
	const std::string&	getHost()       const;
	const std::string&	getPath()       const;
	const std::string&	getQuery()      const;
	const std::string&	getVersion()    const;
	short				getStatusCode() const;
	const HttpError		&getError()		const;
};

#endif