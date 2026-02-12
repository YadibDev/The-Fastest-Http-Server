#include <string>
#include <map>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../Utils/Lexer.hpp"
#include "../../Utils/HelperFunctions.hpp"
#include "../../Utils/HttpError.hpp"
#include "../../ParseRequest/URIParser.hpp"



struct s_parse_context {
	clsParse<TokenType>& parser;
	HttpError&           error;

	s_parse_context(clsParse<TokenType>& p, HttpError& e) 
		: parser(p), error(e) {}
};

class ConfigDirectiveParser {
public:

	static std::string ParseRoot(s_parse_context& ctx) {
		ctx.parser.advance(); 
		if (ctx.parser.peek().type != TOKEN_WORD)
			return (ctx.error.setStatus(400, "Expected path after 'root'"), "");

		std::string root;
		ctx.error = URIParser::normalizePath(ctx.parser.peek().value, root);
		if (ctx.error.isError()) return "";

		ctx.parser.advance();
		if (ctx.parser.peek().type != TOKEN_SEMICOLON)
			return (ctx.error.setStatus(400, "Missing ';' after root"), "");
		
		ctx.parser.advance();
		return root;
	}

	static unsigned long long ParseClientMaxBodySize(s_parse_context& ctx) {
		ctx.parser.advance();
		if (ctx.parser.peek().type != TOKEN_WORD)
			return (ctx.error.setStatus(400, "Expected size value"), 0);

		short numLength = 0;
		long long num = extractNumericPart(ctx.parser.peek().value, numLength);
		std::string unitStr = ctx.parser.peek().value.substr(numLength);
		char unit = (unitStr.empty()) ? ' ' : unitStr[0];

		unsigned long long bytes = convertToBytes(num, unit, ctx.error);
		ctx.parser.advance();

		if (ctx.parser.peek().type != TOKEN_SEMICOLON)
			return (ctx.error.setStatus(400, "Missing ';' after max_body_size"), 0);

		ctx.parser.advance();
		return bytes;
	}

	static std::vector<std::string> ParseIndex(s_parse_context& ctx) {
		std::vector<std::string> indices;
		ctx.parser.advance();

		while (ctx.parser.peek().type == TOKEN_WORD) {
			indices.push_back(ctx.parser.peek().value);
			ctx.parser.advance();
		}

		if (ctx.parser.peek().type != TOKEN_SEMICOLON)
			ctx.error.setStatus(400, "Missing ';' after index list");
		else
			ctx.parser.advance();

		return indices;
	}

	static bool ParseAutoIndex(s_parse_context& ctx) {
		ctx.parser.advance();
		bool result = false;
		std::string val = ctx.parser.peek().value;

		if (val == "on") result = true;
		else if (val == "off") result = false;
		else ctx.error.setStatus(400, "Invalid autoindex value: " + val);

		ctx.parser.advance();
		if (ctx.parser.peek().type != TOKEN_SEMICOLON)
			ctx.error.setStatus(400, "Missing ';' after autoindex");
		else
			ctx.parser.advance();

		return result;
	}

	static std::vector<std::string> ParseMethods(s_parse_context& ctx) {
		//ParseMethods
	}

	static sockaddr_in ParseListen(s_parse_context& ctx) {
		sockaddr_in addr;
		std::memset(&addr, 0, sizeof(sockaddr_in));

		ctx.parser.advance();

		if (ctx.parser.peek().type == TOKEN_WORD) {
			addr = setSockaddr_in(ctx.parser.peek().value, ctx);
		} else {
			ctx.error.setStatus(400, "Syntax Error: Expected address/port after 'listen'");
			return addr;
		}

		ctx.parser.advance();
		if (ctx.parser.peek().type != TOKEN_SEMICOLON) {
			ctx.error.setStatus(400, "Syntax Error: Missing ';' after listen directive");
			return addr;
		}

		ctx.parser.advance();
		return addr;
	}


private:

	static unsigned long long convertToBytes(long long value, char unit, HttpError&	error) {
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
				error.setStatus(400, "Invalid unitType");
	            multiplier = 0;
	            break;
	    }

	    return (unsigned long long)(value * multiplier);
	}

	static long long extractNumericPart(const std::string& str, short &length) {
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


	static sockaddr_in setSockaddr_in(const std::string& input, s_parse_context& ctx) {
		unsigned short port = 0;
		std::string host = "";
		struct sockaddr_in serv_addr;
		std::memset(&serv_addr, 0, sizeof(serv_addr));

		if (URIParser::extractPort(input, port).isError()) {
			ctx.error.setStatus(400, "Invalid Port in: " + input);
			return serv_addr;
		}
		if (URIParser::extractHost(input, host).isError()) {
			ctx.error.setStatus(400, "Invalid Host in: " + input);
			return serv_addr;
		}

		if (host == "*" || host == "") host = "0.0.0.0";
		if (port == 0) {
			ctx.error.setStatus(400, "Port 0 is reserved and cannot be used");
			return serv_addr;
		}

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port);

		uint32_t ipv4 = validateIPWithSystem(host, AF_INET, ctx);
		serv_addr.sin_addr.s_addr = htonl(ipv4);

		return serv_addr;
	}

	static uint32_t validateIPWithSystem(const std::string& ip, int family, s_parse_context& ctx) {
		uint32_t ipv4_raw;
		if (inet_pton(family, ip.c_str(), &ipv4_raw) != 1) {
			ctx.error.setStatus(400, "Invalid IP Address format: " + ip);
			return 0;
		}
		return ntohl(ipv4_raw); 
	}

};