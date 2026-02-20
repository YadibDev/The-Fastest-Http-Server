#ifndef CONFIG_DIRECTIVE_PARSER_HPP
#define CONFIG_DIRECTIVE_PARSER_HPP




#include "../../Header/HeaderFiles.hpp"



enum enBlocksDirective
{
	L_DIR_LISTEN,
	L_DIR_SERVER_NAME,
	L_DIR_LOCATION,
	L_DIR_ROOT,
	L_DIR_INDEX,
	L_DIR_AUTOINDEX,
	L_DIR_ACCEPTED_METHODS,
	L_DIR_CLIENT_MAX_BODY_SIZE,
	L_DIR_RETURN,
	L_DIR_UPLOAD_STORE,
	L_DIR_CGI_PASS,
	L_DIR_ERROR_PAGE,
	L_DIR_UNKNOWN
};

struct s_parse_context {
	clsParse<TokenType>& parser;
	HttpError&           error;

	s_parse_context(clsParse<TokenType>& p, HttpError& e) 
		: parser(p), error(e) {}
};



class ConfigDirectiveParser {
public:

	static bool 							parseLocationPath(s_parse_context& ctx, stlocation& loc);
	static std::string						ParseRoot(s_parse_context& ctx);
	static unsigned long long				ParseClientMaxBodySize(s_parse_context& ctx);
	static std::vector<std::string>			ParseIndex(s_parse_context& ctx);
	static bool								ParseAutoIndex(s_parse_context& ctx);
	static sockaddr_in						ParseListen(s_parse_context& ctx);
	static stReturnData						ParseReturn(s_parse_context& ctx);
	static std::string						ParseUploadStore(s_parse_context& ctx);
	static std::map<short, stErrorPagedata>	ParseErrorPage(s_parse_context& ctx);
	static void								ParseCGI(s_parse_context& ctx, std::map<std::string, std::string>& cgiMap);
	static short							parseMethods(s_parse_context& ctx);


	static void								skipWhitespace(s_parse_context& ctx);

private:
	static bool								isValidPath(s_parse_context& ctx, const std::string& path, bool expectDir);
	static unsigned long long				convertToBytes(long long value, char unit, HttpError& error);
	static long long						extractNumericPart(const std::string& str, short &length);
	static sockaddr_in						setSockaddr_in(const std::string& input);
	static uint32_t							validateIPWithSystem(const std::string& ip, int family);

};

#endif