
#include "LocationConfig.hpp"
#include "../../Utils/Lexer.hpp"

class   clsParseLocation
{
	enum enLocationDirective {
    	L_DIR_ROOT,
    	L_DIR_INDEX,
    	L_DIR_AUTOINDEX,
    	L_DIR_ACCEPTED_METHODS,
    	L_DIR_CLIENT_MAX_BODY_SIZE,
    	L_DIR_RETURN,
    	L_DIR_UPLOAD_PATH,
    	L_DIR_CGI_PASS,
    	L_DIR_ERROR_PAGE,
    	L_DIR_UNKNOWN
	};

	clsParse<TokenType>	_parser;
	clsLocationConfig	Location;
	HttpError			ERROR;



	enLocationDirective getLocationDirectiveType(const std::string& key) {
	    static std::map<std::string, enLocationDirective> directives;

	    if (directives.empty()) {
	        directives["root"]                 = L_DIR_ROOT;
	        directives["index"]                = L_DIR_INDEX;
	        directives["autoindex"]            = L_DIR_AUTOINDEX;
	        directives["allow_methods"]        = L_DIR_ACCEPTED_METHODS;
	        directives["client_max_body_size"] = L_DIR_CLIENT_MAX_BODY_SIZE;
	        directives["return"]               = L_DIR_RETURN;
	        directives["upload_path"]          = L_DIR_UPLOAD_PATH;
	        directives["cgi_pass"]             = L_DIR_CGI_PASS;
	        directives["error_page"]           = L_DIR_ERROR_PAGE;
	    }

	    std::map<std::string, enLocationDirective>::iterator it = directives.find(key);

	    if (it != directives.end())
	        return it->second;

	    return L_DIR_UNKNOWN;
	}


	void ParseLocationDirective()
	{
    	std::string key = parser.peek().value;
    	enLocationDirective dirType = getLocationDirectiveType(key);

    	switch (dirType) {
    	    case L_DIR_ROOT:
    	        ParseRoot(Location);
    	        break;
    	    case L_DIR_INDEX:
    	        ParseIndex(Location);
    	        break;
    	    case L_DIR_AUTOINDEX:
    	        ParseAutoindex(Location);
    	        break;
    	    case L_DIR_ACCEPTED_METHODS:
    	        ParseMethods(Location);
    	        break;
    	    case L_DIR_CLIENT_MAX_BODY_SIZE:
    	        ParseMaxBody(Location);
    	        break;
    	    case L_DIR_RETURN:
    	        ParseReturn(Location);
    	        break;
    	    case L_DIR_UPLOAD_PATH:
    	        ParseUploadPath(Location);
    	        break;
    	    case L_DIR_CGI_PASS:
    	        ParseCgiPass(Location);
    	        break;
    	    case L_DIR_ERROR_PAGE:
    	        ParseErrorPage(Location);
    	        break;
    	    default:
    	        ERROR.setStatus(400, "Syntax Error: Unknown location directive '" + key + "'");
    	        parser.advance(); 
    	        break;
    	}
	}

	public :
	clsParseLocation (clsParse<TokenType>	parser) : _parser(parser)
	{
		
	}
	clsLocationConfig parseLocation()
	{
		_parser.advance();
		Location.setPath(_parser.peek().value);
		if (_parser.advance().type != TOKEN_LBRACE)
			return (ERROR.setStatus(400, "Error in {"), Location);

		while (_parser.peek().type != TOKEN_RBRACE && _parser.peek().type != TOKEN_EOF)
		{
			ParseLocationDirective();
		}
		if (_parser.peek().type != TOKEN_RBRACE)
			return (ERROR.setStatus(400, "Error in }"), Location);
		return (ERROR.setStatus(200, ""), Location);
	}
}