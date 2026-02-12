
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


	bool    ParseRoot() {
    	return (true);
	}

	bool	ParseIndex()
	{
		std::queue<std::string> index;
		if (_parser.advance().type != TOKEN_WORD)
			return (false);
		while (_parser.peek().type != TOKEN_SEMICOLON && _parser.peek().type != TOKEN_EOF)
		{
			index.push(_parser.peek().value);
			_parser.advance();
		}
		if (_parser.peek().type != TOKEN_SEMICOLON)
			return (false);
		Location.setIndex(index);
		return (true);
	}

	bool	ParseAutoIndex() 
	{
	    _parser.advance();
	    if (_parser.peek().type == TOKEN_WORD) 
	    {
	        std::string value = _parser.peek().value;
	        if (value == "on")
	            Location.autoindex = true;
	        else if (value == "off")
	            Location.autoindex = false;
	    	else
	            return (false);
	        _parser.advance();
	    } 
	    else
	        return (false);

	    if (_parser.peek().type != TOKEN_SEMICOLON)
	        return (false);

	    _parser.advance();

	    return (true);
	}

	bool    ParseMethods() {
	    return (true);
	}

	bool    ParseClientMaxBodySize() {
	    return (true);
	}

	bool    ParseReturn() {
	    return (true);
	}

	bool    ParseUploadPath() {
	    return (true);
	}

	bool    ParseCgiPass() {
	    return (true);
	}

	bool    ParseErrorPage() {
	    return (true);
	}

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


	bool ParseLocationDirective()
	{
    	std::string key = parser.peek().value;
    	enLocationDirective dirType = getLocationDirectiveType(key);

    	switch (dirType) {
    	    case L_DIR_ROOT:
    	        return (ParseRoot(Location));
    	        break;
    	    case L_DIR_INDEX:
    	        return (ParseIndex(Location));
    	        break;
    	    case L_DIR_AUTOINDEX:
    	        return (ParseAutoindex(Location));
    	        break;
    	    case L_DIR_ACCEPTED_METHODS:
    	        return (ParseMethods(Location));
    	        break;
    	    case L_DIR_CLIENT_MAX_BODY_SIZE:
    	        return (ParseMaxBody(Location));
    	        break;
    	    case L_DIR_RETURN:
    	        return (ParseReturn(Location));
    	        break;
    	    case L_DIR_UPLOAD_PATH:
    	        return (ParseUploadPath(Location));
    	        break;
    	    case L_DIR_CGI_PASS:
    	        return (ParseCgiPass(Location));
    	        break;
    	    case L_DIR_ERROR_PAGE:
    	        return (ParseErrorPage(Location));
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
			if (!ParseLocationDirective())
				break ;
		}
		if (_parser.peek().type != TOKEN_RBRACE)
			return (ERROR.setStatus(400, "Error in }"), Location);
		return (ERROR.setStatus(200, ""), Location);
	}
}