#pragma once

#include <string>
#include <vector>

class URIParser {
public:

    static std::string decode(std::string str);
    static bool        isRegChar(unsigned char c);

    static bool isValidIPv4(std::string ipv4);
    static bool isValidIPv6(std::string ipv6);
    static bool isValidRegName(const std::string& name);

    static std::string extractScheme(const std::string& uri);
    static std::string extractAuthority(const std::string& uri, const std::string& scheme);
    static std::string extractUserInfo(const std::string& authority);
    static std::string extractHost(const std::string& authority);
    static int         extractPort(const std::string& authority);
    static std::string extractPath(const std::string& uri, const std::string& scheme, const std::string& authority);
    static std::string extractQuery(const std::string& uri);
    static std::string extractFragment(const std::string& uri);

};
