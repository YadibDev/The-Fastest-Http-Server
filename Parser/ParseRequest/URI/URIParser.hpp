#pragma once

#include <string>
#include <vector>
#include "../../../Utils/HttpError.hpp"
#include "../../../Utils/HelperFunctions.hpp"

class URIParser {
    private:
        #define DEFUALT_PORT 80
        static std::string	twoDote(std::string path);
public:

    static std::string	percentDecode(const std::string& str);
    static std::string decode(std::string str);
    static bool        isRegChar(unsigned char c);

    static HttpError isValidIPv4(const std::string& IPv4);
    static HttpError isValidIPv6(const std::string& IPv6);
    static HttpError isValidRegName(const std::string& name);

    static HttpError extractScheme(const std::string& uri, std::string& outScheme);
    static HttpError extractAuthority(const std::string& uri, const std::string& scheme, std::string& outAuthority);
    static HttpError extractUserInfo(const std::string& authority, std::string& outUserInfo);
    static HttpError extractHost(const std::string& authority, std::string& outHost);
    static HttpError extractPort(const std::string& authority, unsigned short &outPort);
    static HttpError extractPath(const std::string& uri, const std::string& authority, std::string& outPath);
    static HttpError extractQuery(const std::string& uri, std::string& outQuery);
    static HttpError extractFragment(const std::string& uri, std::string& outFragment);
    static HttpError normalizePath(const std::string& uri, std::string& outUri);

};
