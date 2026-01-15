#pragma once

#include "URIParser.hpp"
#include <iostream>

class URI
{
private:
    std::string _uri, _scheme, _userInfo, _authority, _host, _path, _query, _fragment;
    int _port;

public:

    URI(std::string uriString);
    const std::string& getScheme()    const;
    const std::string& getHost() const;
    int                getPort() const;
    const std::string& getPath() const;
    const std::string& getQuery() const;
    const std::string& getFragment() const;
};

