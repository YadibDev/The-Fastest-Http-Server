#include "ServerConfig.hpp"

clsServerConfig::clsServerConfig(s_parse_context	&ctxs) : _max_body_size(1048576), ctx(ctxs) {}

clsServerConfig::~clsServerConfig() {}


// Logo Parsing Functions

bool	clsServerConfig::ParseListen()
{
	_listens.push_back(ConfigDirectiveParser::ParseListen(ctx));
	if (ctx.error.isError())
		return (false);
	return (true);
}

// bool	clsServerConfig::ParseServerName()
// {
// 	// لبغيت نزيد server_name
// 	return (true);
// }

bool	clsServerConfig::ParseErrorPage()
{
	_error_pages = ConfigDirectiveParser::ParseErrorPage(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool	clsServerConfig::ParseClientMaxBodySize()
{
	ConfigDirectiveParser::ParseClientMaxBodySize(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool	clsServerConfig::ParseRoot()
{
	_root = ConfigDirectiveParser::ParseRoot(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool	clsServerConfig::ParseIndex()
{
	_index = ConfigDirectiveParser::ParseIndex(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool	clsServerConfig::ParseLocation()
{
	clsLocation loc(ctx);
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
		directives["index"]                 = L_DIR_INDEX;
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

bool    clsServerConfig::ParseServerDirective()
{
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (false);

	std::string directive = ctx.parser.peek().value;
	enBlocksDirective dirType = getServerDirectiveType(directive);

	switch (dirType)
	{
		case L_DIR_LISTEN:					return	ParseListen();
		case L_DIR_ROOT:					return	ParseRoot();
		case L_DIR_INDEX:					return	ParseIndex();
		case L_DIR_CLIENT_MAX_BODY_SIZE:	return	ParseClientMaxBodySize();
		case L_DIR_ERROR_PAGE:				return	ParseErrorPage();
		case L_DIR_LOCATION:				return	ParseLocation();
		default:							return	(false);
	}
}

bool	clsServerConfig::parseBlockServer()
{
	if (ctx.parser.advance().type != TOKEN_LBRACE)
		return (ctx.error.setStatus(400, "Syntax Error: Expected '{' at the beginning of server block"), false);

	ctx.parser.advance();
	while (ctx.parser.peek().type == TOKEN_JOUJNO9ATE)
		ctx.parser.advance();

	while (ctx.parser.peek().type != TOKEN_RBRACE &&
		   ctx.parser.peek().type != TOKEN_EOF)
	{
		if (!ParseServerDirective())
			return (false);
	}


	if (ctx.parser.peek().type != TOKEN_RBRACE)
		return (ctx.error.setStatus(400, "Syntax Error: Expected '}' at the end of server block"), false);

	ctx.parser.advance();
	ConfigDirectiveParser::skipWhitespace(ctx);

	return true;
}

// Getters

std::vector<sockaddr_in> clsServerConfig::getListens() const {
	return _listens;
}

std::map<short, stErrorPagedata> clsServerConfig::getErrorPages() const {
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
	return ctx.error;
}

std::string clsServerConfig::getRoot() const {
	return _root;
}