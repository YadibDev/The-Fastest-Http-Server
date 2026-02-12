#include <string>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ParseRequest/URIParser.hpp"
#include "Utils/Lexer.hpp"
#include "Utils/HttpError.hpp"
#include "Utils/HelperFunctions.hpp"
#include "ParseConfigFile/LocationConfig/ParseLocationConfig.hpp"

enum enServerDirective {
	S_DIR_LISTEN,              
	S_DIR_SERVER_NAME,         
	S_DIR_ERROR_PAGE,          
	S_DIR_CLIENT_MAX_BODY_SIZE,
	S_DIR_ROOT,                
	S_BLOCK_LOCATION,          
	S_DIR_NONE
};



class HttpError {
private:
	int         _codeStatus;
	std::string _msgError;

public:
	HttpError();
	HttpError(int code, std::string msg = "");

	int                getCodeStatus() const;
	const std::string& getMsgError() const;
	bool isError() const;
	void setStatus(int code, std::string msg = "");
};

template <typename TEnum>
class clsParse {
	int _cursor;
	std::vector< Token<TEnum> > _Tokens;
	TEnum _eofType;
public:
	clsParse(std::vector< Token<TEnum> > Tokens, TEnum eofType);
	~clsParse();
	Token<TEnum> peek();
	Token<TEnum> advance();
};

struct stErrorPagedata 
{
    short       response;
    std::string uri;
    stErrorPagedata() : response(0), uri("") {}
};

class ParseServerKey {

	HttpError	ERROR;
public:


	uint32_t validateIPWithSystem(const std::string& ip, int family) {
    
		uint32_t	IPv4;
    	std::memset(&IPv4, 0, sizeof(IPv4));
    	if (inet_pton(family, ip.c_str(), &IPv4) != 1)
    	    ERROR.setStatus(400, "Invalid IP Address format for family");
	
    	ERROR.setStatus(200, "");
		return IPv4;
	}

	sockaddr_in setSockaddr_in(std::string Listen)
	{
		unsigned short	Port;
		std::string		Ip;
		uint32_t		IPv4;
	    struct sockaddr_in serv_addr;
	    std::memset(&serv_addr, 0, sizeof(serv_addr));

	    if (URIParser::extractPort(Listen, Port).isError())
			return (ERROR.setStatus(400, "Invalid Port"), serv_addr);
	    if (URIParser::extractHost(Listen, Ip).isError())
			return (ERROR.setStatus(400, "Invalid IP"), serv_addr);

	    if (Ip == "*" || Ip == "") Ip = "0.0.0.0";
	    if (Port == 0) throw (400);

	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(Port);
	    IPv4 = validateIPWithSystem(Ip, AF_INET);
	    serv_addr.sin_addr.s_addr = htonl(IPv4);
	    return serv_addr;
	}

	sockaddr_in	parseListen(clsParse<TokenType>& parser)
	{
		ERROR.setStatus(200, "");
		sockaddr_in	addr;
		std::memset(&addr, 0, sizeof(sockaddr_in));
		parser.advance();
		if (parser.peek().type == TokenType::TOKEN_WORD)
			addr = setSockaddr_in(parser.peek().value);
		else
			return (ERROR.setStatus(400, "Listen ERROR"), addr);
		parser.advance();
		if (parser.peek().type != TokenType::TOKEN_SEMICOLON)
			return (ERROR.setStatus(400, "Syntax ERROR: Missing ';' after listen"), addr);
		return (addr);
	}

	std::string    ParseRoot(clsParse<TokenType>& parser)
	{
		std::string root = "";
		if (parser.advance().type != TokenType::TOKEN_WORD)
			return (ERROR.setStatus(400, "Syntax ERROR: Expected path after 'root'"), root);
		ERROR = URIParser::normalizePath(parser.peek().value, root);
		if (ERROR.isError())
			return ("");
		if (parser.peek().type != TokenType::TOKEN_SEMICOLON)
			return (ERROR.setStatus(400, "Syntax ERROR: Missing ';' after root path"), "");
		parser.advance();
		return (root);
	}

	unsigned long long convertToBytes(long long value, char unit) {
	    unsigned long long multiplier = 1;

	    switch (unit) {
	        case 'g':
	        case 'G':
	            multiplier = 1024LL * 1024 * 1024;
	            break;
	        case 'm':
	        case 'M':
	            multiplier = 1024LL * 1024;
	            break;
	        case 'k':
	        case 'K':
	            multiplier = 1024LL;
	            break;
			case ' ':
			multiplier = 1;
	            break;
	        default:
				ERROR.setStatus(400, "Invalid unitType");
	            multiplier = 1;
	            break;
	    }

	    return (unsigned long long)(value * multiplier);
	}

	long long extractNumericPart(const std::string& str, short &length) {
	    if (str.empty()) return 0;

	    long long result = 0;
	    size_t i = 0;

	    while (i < str.length() && std::isspace(str[i]))
	        i++;

	    bool foundDigit = false;
	    while (i < str.length() && std::isdigit(str[i])) {
	        int digit = str[i] - '0';

	        if (result > (9223372036854775807LL - digit) / 10)
	            return -1;
	        result = result * 10 + digit;
			length = i;
	        i++;
	        foundDigit = true;
	    }

	    return foundDigit ? result : 0;
	}

	unsigned long long ParseClientMaxBodySize(clsParse<TokenType>& parser) {
    	unsigned long long size = 0;
		
		
		ERROR.setStatus(200, "OK");
    	parser.advance();

    	if (parser.peek().type != TOKEN_WORD)
    	    return (ERROR.setStatus(400, "Syntax Error: Expected size value"), 0);
	
    	const std::string& valStr = parser.peek().value;
    	short numLength = 0;

    	size = extractNumericPart(valStr, numLength);
    	std::string unitStr = valStr.substr(numLength);
	
    	if (unitStr.size() > 1)
    	    return (ERROR.setStatus(400, "Configuration Error: Invalid size unit '" + unitStr + "'"), 0);

    	char unitType = (unitStr.empty()) ? ' ' : unitStr[0];
	
    	size = convertToBytes(size, unitType);

    	parser.advance();

    	if (parser.peek().type != TOKEN_SEMICOLON) {
    	    ERROR.setStatus(400, "Syntax Error: Missing ';' after client_max_body_size");
    	    return 0; 
    	}
	
    	parser.advance();
    	return size;
	}

	std::map<short, stErrorPagedata> ParseErrorPage(clsParse<TokenType>& parser) {
	    std::map<short, stErrorPagedata> errorMap;
	    std::vector<short> codes;
	    short responseOverride = 0;
	    std::string uri = "";
	
	    parser.advance();
	
	    while (parser.peek().type == TOKEN_WORD && is_numeric(parser.peek().value))
		{
	        codes.push_back((short)std::atoi(parser.peek().value.c_str()));
	        parser.advance();
	    }
	
	    if (parser.peek().type == TOKEN_WORD && parser.peek().value[0] == '=')
		{
	        std::string resStr = parser.peek().value;
	        if (resStr.size() > 1)
	            responseOverride = (short)std::atoi(resStr.substr(1).c_str());
	        else
	            responseOverride = -1; 
	        parser.advance();
	    }
	
	    if (parser.peek().type == TOKEN_WORD)
		{
	        uri = parser.peek().value;
	        parser.advance();
	    }
		else
	        return (ERROR.setStatus(400, "Syntax Error: Missing URI in error_page"), errorMap);
	
	    if (parser.peek().type != TOKEN_SEMICOLON) {
	        return (ERROR.setStatus(400, "Syntax Error: Missing ';' after error_page"), errorMap);

	    parser.advance();
	
	    stErrorPagedata data;
	    data.response = responseOverride;
	    data.uri = uri;
	
	    for (size_t i = 0; i < codes.size(); ++i)
	        errorMap[codes[i]] = data;
	
	    return errorMap;
	}

	clsParseLocationConfig	parseLocation(clsParse<TokenType>& parser)
	{
		clsParseLocationConfig ParseLocationConfig();
	}
};



HttpError ParseServerDirective(clsParse<TokenType>& parser) {
	std::string key = parser.peek().value;
	enServerDirective dirType = getServerDirectiveType(key);

	switch (dirType) {
		case S_DIR_LISTEN:
			ParseServerKey::ParseListen(parser);
		case S_DIR_CLIENT_MAX_BODY_SIZE:
			return ParseServerKey::ParseClientMaxBodySize(parser);
		case S_DIR_ERROR_PAGE:
			return ParseServerKey::ParseErrorPage(parser);
		case S_DIR_SERVER_NAME:
		case S_DIR_ROOT:
			return ParseServerKey::ParseRoot(parser);
		case S_BLOCK_LOCATION:
			return HttpError(200, "Location block detected"); 
		default:
			return HttpError(400, "Syntax Error: Unknown server directive '" + key + "'");
	}
}

enServerDirective getServerDirectiveType(const std::string& value) {
	if (value == "listen")                return S_DIR_LISTEN;
	if (value == "server_name")           return S_DIR_SERVER_NAME;
	if (value == "error_page")            return S_DIR_ERROR_PAGE;
	if (value == "client_max_body_size")  return S_DIR_CLIENT_MAX_BODY_SIZE;
	if (value == "root")                  return S_DIR_ROOT;
	if (value == "location")              return S_BLOCK_LOCATION;
	return S_DIR_NONE;
}
