#pragma once

#include <string>
#include <vector>
#include "HttpError.hpp"

class URIParser {
        #define DEFUALT_PORT 80
public:

    static std::string decode(std::string str);
    static bool        isRegChar(unsigned char c);

    static HttpError URIParser::isValidIPv4(const std::string& IPv4);
    static HttpError URIParser::isValidIPv6(const std::string& IPv6);
    static HttpError URIParser::isValidRegName(const std::string& name);

    static HttpError URIParser::extractScheme(const std::string& uri, std::string& outScheme);
    static HttpError URIParser::extractAuthority(const std::string& uri, const std::string& scheme, std::string& outAuthority);
    static HttpError URIParser::extractUserInfo(const std::string& authority, std::string& outUserInfo);
    static HttpError URIParser::extractHost(const std::string& authority, std::string& outHost);
    static HttpError URIParser::extractPort(const std::string& authority, unsigned short &outPort);
    static HttpError URIParser::extractPath(const std::string& uri, const std::string& authority, std::string& outPath);
    static HttpError URIParser::extractQuery(const std::string& uri, std::string& outQuery);
    static HttpError URIParser::extractFragment(const std::string& uri, std::string& outFragment);

};
