#ifndef CONFIG_DIRECTIVE_PARSER_HPP
#define CONFIG_DIRECTIVE_PARSER_HPP


#include "../../Header/HeaderFiles.hpp"

#define BIT_GET_METHOD 1
#define BIT_POST_METHOD 2
#define BIT_DELETE_METHOD 4


enum enBlocksDirective
{
	L_DIR_LISTEN = 5,
	L_DIR_SERVER_NAME,
	L_DIR_LOCATION,
	L_DIR_ROOT,
	L_DIR_INDEX,
	L_DIR_AUTOINDEX,
	L_DIR_ACCEPTED_METHODS,
	L_DIR_CLIENT_MAX_BODY_SIZE,
	L_DIR_RETURN,
	L_DIR_UPLOAD_STORE,
	L_DIR_UPLOAD_LOCATION,
	L_DIR_CGI_PASS,
	L_DIR_ALIAS,
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
	static std::string						ParseUploadLocation(s_parse_context& ctx);
	static bool 							ParseErrorPage(s_parse_context& ctx, std::map<short, stErrorPagedata> &error_pages);
	static void								ParseCGI(s_parse_context& ctx, std::map<std::string, std::string>& cgiMap);
	static short							parseMethods(s_parse_context& ctx);
	static std::string						parseAlias(s_parse_context& ctx);


	static void								skipWhitespace(clsParse<TokenType>& parser);
	static void								DefineUri(s_uri_entry &uri);

private:
	static bool validateDirectoryPath(const std::string& path, s_parse_context& ctx, 
                                  const std::string& directiveName);

	static unsigned long long				convertToBytes(long long value, char unit, HttpError& error);
	static long long						extractNumericPart(const std::string& str, short &length);
	static sockaddr_in 							setSockaddr_in(const std::string& input, HttpError& error);
	static uint32_t							validateIPWithSystem(const std::string& ip, int family);
};

#endif