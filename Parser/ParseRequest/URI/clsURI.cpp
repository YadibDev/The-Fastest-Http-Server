#include <string>
#include <cctype>
#include <iostream>

class URI {
private:

	#define PIECES 4
	#define MAX_IPV6_STR_LEN 39 + 7
	#define VALIDATE_IPV6_SIZE(ip) ((ip).size() <= MAX_IPV6_STR_LEN)
	std::string		_URI;
	std::string		_Scheme;
	std::string		_Authority;
	std::string		_userInfo;
	std::string		_Path;
	std::string		_Host;
	int				_Port;
	std::string		_Query;
	std::string		_Fragment;

	std::string _decode(std::string str) {
    	std::string	res;
    	int			value;
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

	bool _parseIPv4(std::string IPv4) {
	    if (IPv4.empty() || IPv4.size() < 7 || IPv4.size() > 15) return false;

	    int octets = 0;
	    int currentVal = 0;
	    int digitCount = 0;

	    for (size_t i = 0; i < IPv4.size(); ++i) {
	        if (std::isdigit(IPv4[i])) {
	            if (digitCount == 1 && currentVal == 0) return false;

	            currentVal = currentVal * 10 + (IPv4[i] - '0');
	            digitCount++;

	            if (currentVal > 255 || digitCount > 3) return false;
	        } 
	        else if (IPv4[i] == '.') {
	            if (digitCount == 0 || octets == 3) return false;
	
	            octets++;
	            currentVal = 0;
	            digitCount = 0;
	        } 
	        else return false;
	    }
	    return (octets == 3 && digitCount > 0);
	}

	bool _parseIPv6(std::string IPv6)
	{
	    if (IPv6.size() < 2 || !VALIDATE_IPV6_SIZE(IPv6))
	        return false;
	
	    if ((IPv6[0] == ':' && IPv6[1] != ':') || 
	        (IPv6.back() == ':' && IPv6[IPv6.size() - 2] != ':'))
	        return false;
	
	    bool hasDoubleColon = false;
	    short pieceCount = 0;
	    short i = 0;
	    while (i < IPv6.size())
	    {
	        if (IPv6[i] == ':')
	        {
	            if (i + 1 < IPv6.size() && IPv6[i + 1] == ':')
	            {
	                if (hasDoubleColon)
	                    return false;
	                hasDoubleColon = true;
	                i += 2;
	                continue;
	            }
	            i++;
	            continue;
	        }
	        unsigned char hexCount = 0;
	        while (i < IPv6.size() && IPv6[i] != ':')
	        {
	            if (!std::isxdigit(IPv6[i]))
	                return false;
	            if (++hexCount > 4)
	                return false;
	            i++;
	        }
	        if (hexCount == 0)
	            return false;
	        pieceCount++;
	    }
	    return pieceCount > 0;
	}

	std::string _parseScheme() {
		size_t colonPos = _URI.find(':');
		size_t slashPos = _URI.find('/');

		if (colonPos == std::string::npos || (colonPos + 1 != slashPos)
			|| (slashPos != std::string::npos && colonPos > slashPos))
			return "";

		std::string Scheme = _URI.substr(0, colonPos);

		if (Scheme.empty() || !std::isalpha(Scheme[0]))
			return "";

		for (size_t i = 0; i < Scheme.length(); ++i) {
			char c = Scheme[i];
			if (!std::isalnum(c) && c != '+' && c != '-' && c != '.')
				return "";
			Scheme[i] = std::tolower(c);
		}
		return Scheme;
	}

	std::string _parseAuthority() { 
	
		if (_Scheme.empty())
			return "";
		size_t authorityStart = _URI.find("://");
    	if (authorityStart == std::string::npos) return "";
    	authorityStart += 3;

    	size_t authorityEnd = _URI.find_first_of("/?#", authorityStart);
    	if (authorityEnd == std::string::npos) 
    	    authorityEnd = _URI.length();

    	std::string authorityPart = _URI.substr(authorityStart, authorityEnd - authorityStart);
		return authorityPart;
	}

	std::string _parseUserInfo(std::string Authority) {
    	if (_Scheme.empty()) return "";
	
    	size_t atPos = Authority.find('@');
    	if (atPos == std::string::npos)
    	    return "";
    	return _decode(Authority.substr(0, atPos));
	}

	bool _isRegChar(unsigned char c) {
	    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
	        return true;
	
	    switch (c) {
	        case '-': case '.': case '_': case '~': case '!': case '$':
	        case '&': case '\'': case '(': case ')': case '*': case '+':
	        case ',': case ';': case '=':
	            return true;
	        default:
	            return false;
	    }
	}

	bool _parseRegName(const std::string& name) {
	    for (size_t i = 0; i < name.size(); ++i) {
	        unsigned char c = name[i];
	        if (_isRegChar(c)) continue;

	        if (c == '%' && i + 2 < name.size() && 
	            std::isxdigit(static_cast<unsigned char>(name[i+1])) && 
	            std::isxdigit(static_cast<unsigned char>(name[i+2]))) {
	            i += 2;
	            continue;
	        }
	        return false;
	    }
	    return true;
	}

	std::string _parseHost(const std::string& Authority) {
	    if (Authority.empty()) return "";

	    size_t start = Authority.find_last_of('@');
	    start = (start == std::string::npos) ? 0 : start + 1;

	    if (start < Authority.size() && Authority[start] == '[') {
	        size_t endBracket = Authority.find(']', start);
	        if (endBracket == std::string::npos) return "";
	
	        std::string ip = Authority.substr(start + 1, endBracket - start - 1);
	        return (_parseIPv6(ip)) ? "[" + ip + "]" : "";
	    }

	    size_t colonPos = Authority.find_last_of(':');
	    size_t hostEnd = (colonPos != std::string::npos && colonPos > start) ? colonPos : Authority.size();
	
	    std::string host = Authority.substr(start, hostEnd - start);
	    if (host.empty()) return "";

	    if (std::isdigit(static_cast<unsigned char>(host[0])) && _parseIPv4(host))
	        return host;

	    return _parseRegName(host) ? host : "";
	}

	int _parsePort(std::string Authority)
	{
	    size_t colonPos = Authority.find_last_of(':');
	
	    if (colonPos == std::string::npos)
	        return -1;
	
	    std::string portStr = Authority.substr(colonPos + 1);
	
	    if (portStr.empty())
	        return -1;
	    for (size_t i = 0; i < portStr.length(); i++)
	    {
	        if (!std::isdigit(portStr[i]))
	            return -1;
	    }    
	    unsigned long portNum = std::strtoul(portStr.c_str(), NULL, 10);

	    if (portNum > 65535)
	        return -1;
	    return static_cast<unsigned short>(portNum);
	}
	
	std::string _parsePath()
	{
	    size_t pathStart = 0;
	    size_t pathEnd = _URI.size();
	
	    if (!_Scheme.empty())
	    {
	        pathStart = _Scheme.size() + 1;
	        if (!_Authority.empty())
	            pathStart += 2 + _Authority.size();
	    }
	
	    size_t endDelim = _URI.find_first_of("?#", pathStart);
	    if (endDelim != std::string::npos)
	        pathEnd = endDelim;
	
	    if (pathStart >= pathEnd)
	        return "";
	
	    return _decode(_URI.substr(pathStart, pathEnd - pathStart));
	}

	std::string _parseQuery()
	{
    	size_t queryStart = _URI.find('?');

    	if (queryStart == std::string::npos)
    	    return "";
	
    	size_t fragmentStart = _URI.find('#', queryStart + 1);

    	if (fragmentStart == std::string::npos)
    	    return _URI.substr(queryStart + 1);
    	return _decode(_URI.substr(queryStart + 1, fragmentStart - (queryStart + 1)));
	}
	
	std::string _parseFragment()
	{
	    size_t hashPos = _URI.find('#');
	
	    if (hashPos == std::string::npos)
	        return "";
	
	    if (hashPos + 1 >= _URI.length())
	        return "";
	
	    return _decode(_URI.substr(hashPos + 1));
	}

	void parsingURI() {
		_Scheme    = _parseScheme(); //COMPLETE
		_Authority = _parseAuthority(); //COMPLETE
		_userInfo  = _parseUserInfo(_Authority); //COMPLETE
		_Host      = _parseHost(_Authority);  //COMPLETE
		_Port      = _parsePort(_Authority); //COMPLETE
		_Path      = _parsePath(); //COMPLETE
		_Query     = _parseQuery(); //COMPLETE
		_Fragment  = _parseFragment(); //COMPLETE
	}

public:
	URI(std::string uriString) : _URI(uriString) {
		parsingURI();
	}

	const std::string& getFullURI()   const { return _URI; }
	const std::string& getScheme()    const { return _Scheme; }
	const std::string& getAuthority() const { return _Authority; }
	const std::string& getUserInfo()  const { return _userInfo; }
	const std::string& getPath()      const { return _Path; }
	const std::string& getHost()      const { return _Host; }
	int                getPort()      const { return _Port; }
	const std::string& getQuery()     const { return _Query; }
	const std::string& getFragment()  const { return _Fragment; }
};
