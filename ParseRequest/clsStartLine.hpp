#ifndef CLSSTARTLINE_HPP
#define CLSSTARTLINE_HPP

#include <string>
#include "URI.hpp"
#include "HeaderRequest.hpp"

class clsStartLine {
public:

private:
    bool        _isAbsoluteURI;
    eMethods    _method;
    std::string _scheme;
    std::string _host;
    std::string _port;
    std::string _path;
    std::string _query;
    std::string _version;
    short       statuCode;

    bool        _parseMethod(std::string methodStr);
    bool        _parseVersion(std::string version);
    void        _parsURI(std::string URI_str);
    std::string getPart(std::string str, size_t &indx);
    void        _parseStartLine(std::string startLine);

    void        skipWhitespace(std::string str, size_t &indx);
    bool        isCRLF(std::string str, size_t indx);

public:
    clsStartLine(std::string startLine);
    
    // Getters
    bool                isAbsoluteURI() const;
    eMethods            getMethod()     const;
    const std::string&  getHost()       const;
    const std::string&  getPath()       const;
    const std::string&  getQuery()      const;
    const std::string&  getVersion()    const;
    short               getStatusCode() const;
};

#endif