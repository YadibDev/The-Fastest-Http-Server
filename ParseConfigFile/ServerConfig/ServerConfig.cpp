#include "ServerConfig.hpp"

clsServerConfig::clsServerConfig(clsParse<TokenType>	Parse) : _max_body_size(1048576), _Parse(Parse) {}

clsServerConfig::~clsServerConfig() {}


// Logo Parsing Functions

bool	clsServerConfig::ParseListen(s_parse_context& ctx)
{
	ConfigDirectiveParser::ParseListen(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

// bool	clsServerConfig::ParseServerName(s_parse_context& ctx)
// {
// 	// لبغيت نزيد server_name
// 	return (true);
// }

bool	clsServerConfig::ParseErrorPage(s_parse_context& ctx)
{
	ConfigDirectiveParser::ParseErrorPage(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool	clsServerConfig::ParseClientMaxBodySize(s_parse_context& ctx)
{
	ConfigDirectiveParser::ParseClientMaxBodySize(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool	clsServerConfig::ParseRoot(s_parse_context& ctx)
{
	ConfigDirectiveParser::ParseRoot(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool	clsServerConfig::ParseLocation(s_parse_context& ctx)
{
	clsLocation loc(_Parse);
	loc.parseLocation();
	if (!loc.getError().isError())
	{
		if (loc.getLocationData().matchType == stlocation::EXACT)
			_LocationExact.push_back(loc);
		else
			_LocationPrefix.push_back(loc);
	}
	else
	{
		ctx.error = loc.getError();
		return false;
	}
	return (true);
}



// Main parsing function
enBlocksDirective	clsServerConfig::getServerDirectiveType(const std::string& key)
{
	static std::map<std::string, enBlocksDirective> directives;

	if (directives.empty())
	{
		directives["listen"]               = L_DIR_LISTEN;
		directives["root"]                  = L_DIR_ROOT;
		directives["client_max_body_size"] = L_DIR_CLIENT_MAX_BODY_SIZE;
		directives["error_page"]           = L_DIR_ERROR_PAGE;
		directives["location"]             = L_DIR_LOCATION;
	}

	std::map<std::string, enBlocksDirective>::iterator it =
		directives.find(key);

	if (it != directives.end())
		return it->second;

	return L_DIR_UNKNOWN;
}

bool    clsServerConfig::ParseServerDirective(s_parse_context& ctx)
{
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (false);

	std::string directive = ctx.parser.peek().value;
	enBlocksDirective dirType = getServerDirectiveType(directive);

	switch (dirType)
	{
		case L_DIR_LISTEN:					return	ParseListen(ctx);
		case L_DIR_ROOT:					return	ParseRoot(ctx);
		case L_DIR_CLIENT_MAX_BODY_SIZE:	return	ParseClientMaxBodySize(ctx);
		case L_DIR_ERROR_PAGE:				return	ParseErrorPage(ctx);
		case L_DIR_LOCATION:				return	ParseLocation(ctx);
		default:							return	(false);
	}
}

bool	clsServerConfig::parseBlockServer()
{

	s_parse_context ctx(_Parse, _ERROR);

	if (_Parse.advance().type != TOKEN_LBRACE)
		return (_ERROR.setStatus(400, "Syntax Error: Expected '{' at the beginning of server block"), false);

	while (_Parse.peek().type != TOKEN_RBRACE &&
		   _Parse.peek().type != TOKEN_EOF)
	{
		if (!ParseServerDirective(ctx))
			return (_ERROR.setStatus(400, "Unknown directive in server block: " + _Parse.peek().value), false);
	}

	if (_Parse.peek().type != TOKEN_RBRACE)
		return (_ERROR.setStatus(400, "Syntax Error: Expected '}' at the end of server block"), false);

	return true;
}

// Getters

std::vector<ListenInfo> clsServerConfig::getListens() const {
	return _listens;
}

std::map<int, stErrorPagedata> clsServerConfig::getErrorPages() const {
	return _error_pages;
}

size_t clsServerConfig::getMaxBodySize() const {
	return _max_body_size;
}

std::vector<clsLocation> clsServerConfig::getLocationExact() const {
	return _LocationExact;
}

std::vector<clsLocation> clsServerConfig::getLocationPrefix() const {
	return _LocationPrefix;
}

HttpError clsServerConfig::getError() const {
	return _ERROR;
}