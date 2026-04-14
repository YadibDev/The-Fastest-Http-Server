#include "URIParser.hpp"
#include <cstdio>
#include <cstdlib>

std::string	URIParser::decode(std::string str) {
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

HttpError	URIParser::isValidIPv6(const std::string& IPv6) {
	if (IPv6.size() < 2)
		return HttpError(400, "IPv6 address is too short");

	if ((IPv6[0] == ':' && IPv6[1] != ':') || 
		((IPv6[(IPv6.size() - 1)]) == ':' && IPv6[IPv6.size() - 2] != ':')) {
		return HttpError(400, "Invalid colon placement in IPv6");
	}
	bool hasDoubleColon = false;
	short pieceCount = 0, i = 0;
	while (i < (short)IPv6.size()) {
		if (IPv6[i] == ':') {
			if (i + 1 < (short)IPv6.size() && IPv6[i + 1] == ':') {
				if (hasDoubleColon) 
					return HttpError(400, "Multiple double colons (::) are not allowed");
				hasDoubleColon = true;
				i += 2; 
				pieceCount++;
				continue;
			}
			i++;
			continue;
		}
		unsigned char hexCount = 0;
		while (i < (short)IPv6.size() && IPv6[i] != ':') {
			if (!std::isxdigit(IPv6[i])) 
				return HttpError(400, "IPv6 contains non-hex characters");
			if (++hexCount > 4)
				return HttpError(400, "IPv6 hex group exceeds 4 digits");
			i++;
		}
		
		if (hexCount == 0) 
			return HttpError(400, "Empty hex group in IPv6");
		pieceCount++;
	}

	if (!pieceCount)
		return HttpError(400, "Invalid IPv6 piece count");

	return HttpError();
}

HttpError	URIParser::isValidIPv4(const std::string& IPv4) {
	if (IPv4.empty()) 
		return HttpError(400, "IPv4 address is empty");
	if (IPv4.size() < 7 || IPv4.size() > 15) 
		return HttpError(400, "IPv4 length is invalid (must be 7-15 chars)");

	int octets = 0, currentVal = 0, digitCount = 0;
	for (size_t i = 0; i < IPv4.size(); ++i) {
		if (std::isdigit(IPv4[i])) {
			if (digitCount == 1 && currentVal == 0) 
				return HttpError(400, "Leading zeros are not allowed in IPv4");
			
			currentVal = currentVal * 10 + (IPv4[i] - '0');
			digitCount++;
			
			if (currentVal > 255) 
				return HttpError(400, "Octet value exceeds 255");
			if (digitCount > 3) 
				return HttpError(400, "Too many digits in one octet");
		} 
		else if (IPv4[i] == '.') {
			if (digitCount == 0) 
				return HttpError(400, "Empty octet between dots");
			if (octets == 3) 
				return HttpError(400, "Too many dots in IPv4");
			
			octets++;
			currentVal = 0;
			digitCount = 0;
		} 
		else
			return HttpError(400, "IPv4 contains invalid characters");
	}
	if (octets != 3 || digitCount == 0)
		return HttpError(400, "Invalid IPv4 format (must be x.x.x.x)");
	return HttpError();
}

HttpError	URIParser::extractScheme(const std::string& uri, std::string& outScheme) {
	size_t colonPos = uri.find(':');
	size_t slashPos = uri.find('/');

	if (colonPos == std::string::npos)
		return HttpError(400, "Missing scheme separator ':'");
	if (colonPos + 1 != slashPos)
		return HttpError(400, "Invalid scheme format: colon must be followed by '/'");
	if (slashPos != std::string::npos && colonPos > slashPos)
		return HttpError(400, "Scheme colon found after first slash");
	std::string scheme = uri.substr(0, colonPos);
	if (scheme.empty() || !std::isalpha(static_cast<unsigned char>(scheme[0])))
		return HttpError(400, "Scheme must start with an alphabetic character");
	for (size_t i = 0; i < scheme.length(); ++i) {
		char c = scheme[i];
		if (!std::isalnum(static_cast<unsigned char>(c)) && c != '+' && c != '-' && c != '.')
			return HttpError(400, "Scheme contains invalid characters");
		scheme[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	outScheme = scheme;
	return HttpError();
}

HttpError	URIParser::extractAuthority(const std::string& uri, const std::string& scheme, std::string& outAuthority) {
	if (scheme.empty())
		return HttpError(400, "Cannot extract authority: Scheme is empty");

	size_t authorityStart = uri.find("://");
	if (authorityStart == std::string::npos)
		return HttpError(400, "Missing authority separator '://'");
	authorityStart += 3;
	size_t authorityEnd = uri.find_first_of("/?#", authorityStart);
	if (authorityEnd == std::string::npos)
		authorityEnd = uri.length();
	outAuthority = uri.substr(authorityStart, authorityEnd - authorityStart);
	return HttpError();
}

HttpError	URIParser::extractUserInfo(const std::string& authority, std::string& outUserInfo) {
	outUserInfo = "";

	if (authority.empty())
		return HttpError();
	size_t atPos = authority.find('@');
	if (atPos == std::string::npos)
		return HttpError();
	std::string encodedUserInfo = authority.substr(0, atPos);
	outUserInfo = decode(encodedUserInfo);
	return HttpError();
}

bool		URIParser::isRegChar(unsigned char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
		return true;
	switch (c) {
		case '-': case '.': case '_': case '~': case '!': case '$':
		case '&': case '\'': case '(': case ')': case '*': case '+':
		case ',': case ';': case '=': return true;
		default: return false;
	}
}

HttpError	URIParser::isValidRegName(const std::string& name) {
	if (name.empty())
		return HttpError(400, "Registered name is empty");

	for (size_t i = 0; i < name.size(); ++i) {
		unsigned char c = name[i];
		if (c == '%') {
			if (i + 2 < name.size() && 
				std::isxdigit(static_cast<unsigned char>(name[i+1])) && 
				std::isxdigit(static_cast<unsigned char>(name[i+2]))) {
				i += 2;
				continue;
			}
			return HttpError(400, "Invalid percent-encoding in Host name");
		}
		if (isRegChar(c)) 
			continue;
		return HttpError(400, "Host name contains forbidden characters");
	}
	return HttpError();
}

HttpError	URIParser::extractHost(const std::string& authority, std::string& outHost) {
	if (authority.empty()) 
		return HttpError(400, "Authority is empty");

	size_t start = authority.find_last_of('@');
	start = (start == std::string::npos) ? 0 : start + 1;

	if (start < authority.size() && authority[start] == '[') {
		size_t endBracket = authority.find(']', start);
		if (endBracket == std::string::npos) 
			return HttpError(400, "Missing closing bracket for IPv6");
		outHost = authority.substr(start + 1, endBracket - start - 1);
		return HttpError();
	}
	size_t colonPos = authority.find_last_of(':');
	size_t hostEnd = (colonPos != std::string::npos && colonPos > start) ? colonPos : authority.size();
	
	outHost = authority.substr(start, hostEnd - start);
	
	if (outHost.empty())
		return HttpError();

	if (std::isdigit(static_cast<unsigned char>(outHost[0])))
		return HttpError();

	return isValidRegName(outHost);
}

HttpError	URIParser::extractPort(const std::string& authority, unsigned short &outPort) {

	size_t colonPos = authority.find_last_of(':');
	outPort = 80;

	size_t  isIPv6 = authority.find_first_of('[');
	if (isIPv6 != std::string::npos)
	{
		isIPv6 = authority.find_last_of(']');
		std::string withoutHost = authority.substr(isIPv6 + 1);
		colonPos = isIPv6 + withoutHost.find_first_of(':') + 1;
	}
	if (colonPos == std::string::npos) {
		outPort = DEFUALT_PORT;
		return HttpError(); 
	}
	std::string portStr = authority.substr(colonPos + 1);
	if (portStr.empty())
		return HttpError();

	for (size_t i = 0; i < portStr.length(); i++) {
		if (!std::isdigit(portStr[i])) 
			return HttpError(400, "Port must be numeric");
	}
	unsigned long portNum = std::strtoul(portStr.c_str(), NULL, 10);
	if (portNum > 65535) 
		return HttpError(400, "Port out of range (max 65535)");
	outPort = static_cast<int>(portNum);
	return HttpError();
}

HttpError	URIParser::extractPath(const std::string& uri, const std::string& authority, std::string& outPath) {
	size_t pathStart = 0;
	if (authority.empty())
		pathStart = 0; 
	else {
		size_t authPos = uri.find(authority);
		if (authPos == std::string::npos)
			return HttpError(400, "Authority not found in URI");
		pathStart = authPos + authority.length();
	}
	size_t endPos = uri.find_first_of("?#", pathStart);
	if (endPos == std::string::npos)
		endPos = uri.length();
	std::string rawPath = uri.substr(pathStart, endPos - pathStart);
	outPath = decode(rawPath);
	return HttpError();
}

HttpError	URIParser::extractQuery(const std::string& uri, std::string& outQuery) {
	outQuery = "";
	
	size_t queryStart = uri.find('?');
	if (queryStart == std::string::npos)
		return HttpError();

	size_t fragmentStart = uri.find('#', queryStart + 1);
	std::string rawQuery;

	if (fragmentStart == std::string::npos)
		rawQuery = uri.substr(queryStart + 1);
	else
		rawQuery = uri.substr(queryStart + 1, fragmentStart - (queryStart + 1));
	outQuery = decode(rawQuery);
	return HttpError();
}

HttpError	URIParser::extractFragment(const std::string& uri, std::string& outFragment) {
	outFragment = "";

	size_t hashPos = uri.find('#');
	
	if (hashPos == std::string::npos || hashPos + 1 >= uri.length())
		return HttpError();
	std::string rawFragment = uri.substr(hashPos + 1);
	outFragment = decode(rawFragment);

	return HttpError();
}

#include <string>

inline int from_hex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

void decode_segment(const std::string& uri, size_t start, size_t end, std::string& buffer) {
    buffer.clear();
    for (size_t j = start; j < end; ++j) {
        if (uri[j] == '%' && j + 2 < end) {
            int h1 = from_hex(uri[j + 1]);
            int h2 = from_hex(uri[j + 2]);
            if (h1 != -1 && h2 != -1) {
                buffer += static_cast<char>((h1 << 4) | h2);
                j += 2;
            } else {
                buffer += uri[j];
            }
        } else {
            buffer += uri[j];
        }
    }
}

void resolve_segment(const std::string& segment, std::string& outUri) {
    if (segment == ".") return;
    
    if (segment == "..") {
        if (!outUri.empty()) {
            size_t lastSlash = outUri.rfind('/');
            if (lastSlash != std::string::npos) {
                outUri.resize(lastSlash);
            } else {
                outUri.clear();
            }
        }
        return;
    }
    outUri += '/';
    outUri += segment;
}

HttpError URIParser::normalizePath(const std::string& uri, std::string& outUri) {
    if (uri.empty()) {
        outUri = "/";
        return HttpError();
    }

    outUri.clear();
    outUri.reserve(uri.length());

    std::string decodedBuffer;
    decodedBuffer.reserve(256);

    size_t len = uri.length();
    size_t i = 0;

    while (i < len) {
        if (uri[i] == '/') {
            i++;
            continue;
        }

        size_t start = i;
        while (i < len && uri[i] != '/')
            i++;
        decode_segment(uri, start, i, decodedBuffer);
        resolve_segment(decodedBuffer, outUri);
    }

    if (outUri.empty())
        outUri = "/";

    if (len > 1 && uri[len - 1] == '/' && outUri[outUri.size() - 1] != '/')
        outUri += '/';

    return HttpError();
}

short	URIParser::getMaxUriLength()
{
    short MAX_URI_LENGTH = 8192;

	return MAX_URI_LENGTH;
}