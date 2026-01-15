#include "URIParser.hpp"
#include <cstdio>
#include <cstdlib>

std::string URIParser::decode(std::string str) {
    std::string res;
    int         value;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::sscanf(str.substr(i + 1, 2).c_str(), "%x", &value);
            res += static_cast<char>(value);
            i += 2;
        }
        else
            res += str[i];
    }
    return res;
}

bool URIParser::isValidIPv4(std::string IPv4) {
    if (IPv4.empty() || IPv4.size() < 7 || IPv4.size() > 15) return false;
    int octets = 0, currentVal = 0, digitCount = 0;
    for (size_t i = 0; i < IPv4.size(); ++i) {
        if (std::isdigit(IPv4[i])) {
            if (digitCount == 1 && currentVal == 0) return false;
            currentVal = currentVal * 10 + (IPv4[i] - '0');
            digitCount++;
            if (currentVal > 255 || digitCount > 3) return false;
        } 
        else if (IPv4[i] == '.') {
            if (digitCount == 0 || octets == 3) return false;
            octets++; currentVal = 0; digitCount = 0;
        } 
        else return false;
    }
    return (octets == 3 && digitCount > 0);
}

bool URIParser::isValidIPv6(std::string IPv6) {
    if (IPv6.size() < 2) return false; 

    if ((IPv6[0] == ':' && IPv6[1] != ':') || 
        (IPv6.back() == ':' && IPv6[IPv6.size() - 2] != ':'))
        return false;

    bool hasDoubleColon = false;
    short pieceCount = 0, i = 0;
    while (i < (short)IPv6.size()) {
        if (IPv6[i] == ':') {
            if (i + 1 < (short)IPv6.size() && IPv6[i + 1] == ':') {
                if (hasDoubleColon) return false;
                hasDoubleColon = true;
                i += 2; continue;
            }
            i++; continue;
        }
        unsigned char hexCount = 0;
        while (i < (short)IPv6.size() && IPv6[i] != ':') {
            if (!std::isxdigit(IPv6[i])) return false;
            if (++hexCount > 4) return false;
            i++;
        }
        if (hexCount == 0) return false;
        pieceCount++;
    }
    return pieceCount > 0;
}

std::string URIParser::extractScheme(const std::string& uri) {
    size_t colonPos = uri.find(':');
    size_t slashPos = uri.find('/');
    if (colonPos == std::string::npos || (colonPos + 1 != slashPos)
        || (slashPos != std::string::npos && colonPos > slashPos))
        return "";
    std::string Scheme = uri.substr(0, colonPos);
    if (Scheme.empty() || !std::isalpha(Scheme[0])) return "";
    for (size_t i = 0; i < Scheme.length(); ++i) {
        char c = Scheme[i];
        if (!std::isalnum(c) && c != '+' && c != '-' && c != '.') return "";
        Scheme[i] = std::tolower(c);
    }
    return Scheme;
}

std::string URIParser::extractAuthority(const std::string& uri, const std::string& scheme) {
    if (scheme.empty()) return "";
    size_t authorityStart = uri.find("://");
    if (authorityStart == std::string::npos) return "";
    authorityStart += 3;
    size_t authorityEnd = uri.find_first_of("/?#", authorityStart);
    if (authorityEnd == std::string::npos) authorityEnd = uri.length();
    return uri.substr(authorityStart, authorityEnd - authorityStart);
}

std::string URIParser::extractUserInfo(const std::string& authority) {
    size_t atPos = authority.find('@');
    if (atPos == std::string::npos) return "";
    return decode(authority.substr(0, atPos));
}

bool URIParser::isRegChar(unsigned char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        return true;
    switch (c) {
        case '-': case '.': case '_': case '~': case '!': case '$':
        case '&': case '\'': case '(': case ')': case '*': case '+':
        case ',': case ';': case '=': return true;
        default: return false;
    }
}

bool URIParser::isValidRegName(const std::string& name) {
    for (size_t i = 0; i < name.size(); ++i) {
        unsigned char c = name[i];
        if (isRegChar(c)) continue;
        if (c == '%' && i + 2 < name.size() && 
            std::isxdigit(static_cast<unsigned char>(name[i+1])) && 
            std::isxdigit(static_cast<unsigned char>(name[i+2]))) {
            i += 2; continue;
        }
        return false;
    }
    return true;
}

std::string URIParser::extractHost(const std::string& authority) {
    if (authority.empty()) return "";
    size_t start = authority.find_last_of('@');
    start = (start == std::string::npos) ? 0 : start + 1;
    if (start < authority.size() && authority[start] == '[') {
        size_t endBracket = authority.find(']', start);
        if (endBracket == std::string::npos) return "";
        std::string ip = authority.substr(start + 1, endBracket - start - 1);
        return (isValidIPv6(ip)) ? "[" + ip + "]" : "";
    }
    size_t colonPos = authority.find_last_of(':');
    size_t hostEnd = (colonPos != std::string::npos && colonPos > start) ? colonPos : authority.size();
    std::string host = authority.substr(start, hostEnd - start);
    if (host.empty()) return "";
    if (std::isdigit(static_cast<unsigned char>(host[0])) && isValidIPv4(host))
        return host;
    return isValidRegName(host) ? host : "";
}

int URIParser::extractPort(const std::string &authority) {
    size_t colonPos = authority.find_last_of(':');
    if (colonPos == std::string::npos) return -1;
    std::string portStr = authority.substr(colonPos + 1);
    if (portStr.empty()) return -1;
    for (size_t i = 0; i < portStr.length(); i++) {
        if (!std::isdigit(portStr[i])) return -1;
    }    
    unsigned long portNum = std::strtoul(portStr.c_str(), NULL, 10);
    if (portNum > 65535) return -1;
    return static_cast<int>(portNum);
}

std::string URIParser::extractPath(const std::string& uri, const std::string& scheme, const std::string& authority) {
    size_t pathStart = 0;
    size_t pathEnd = uri.size();
    if (!scheme.empty()) {
        pathStart = scheme.size() + 1;
        if (!authority.empty()) pathStart += 2 + authority.length();
    }
    size_t endDelim = uri.find_first_of("?#", pathStart);
    if (endDelim != std::string::npos) pathEnd = endDelim;
    if (pathStart >= pathEnd) return "";
    return decode(uri.substr(pathStart, pathEnd - pathStart));
}

std::string URIParser::extractQuery(const std::string& uri) {
    size_t queryStart = uri.find('?');
    if (queryStart == std::string::npos) return "";
    size_t fragmentStart = uri.find('#', queryStart + 1);
    if (fragmentStart == std::string::npos)
        return decode(uri.substr(queryStart + 1));
    return decode(uri.substr(queryStart + 1, fragmentStart - (queryStart + 1)));
}

std::string URIParser::extractFragment(const std::string& uri) {
    size_t hashPos = uri.find('#');
    if (hashPos == std::string::npos || hashPos + 1 >= uri.length())
        return "";
    return decode(uri.substr(hashPos + 1));
}

