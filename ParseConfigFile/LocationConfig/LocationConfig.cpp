#include "LocationConfig.hpp"


bool	clsLocation::ParseRoot()
{
	_root = ConfigDirectiveParser::ParseRoot(ctx);
	return !ctx.error.isError();
}

bool	clsLocation::ParseIndex()
{
	_index = ConfigDirectiveParser::ParseIndex(ctx);
	return !ctx.error.isError();
}

bool	clsLocation::ParseAutoIndex()
{
	_autoindex = ConfigDirectiveParser::ParseAutoIndex(ctx);
	return !ctx.error.isError();
}

bool	clsLocation::ParseMethods()
{
	_allow_methods = ConfigDirectiveParser::parseMethods(ctx);
	return !ctx.error.isError();
}

bool	clsLocation::ParseClientMaxBodySize()
{
	_client_max_body_size = ConfigDirectiveParser::ParseClientMaxBodySize(ctx);
	return !ctx.error.isError();
}

bool	clsLocation::ParseReturn()
{
	_return = ConfigDirectiveParser::ParseReturn(ctx);
	return !ctx.error.isError();
}

bool	clsLocation::ParseUploadPath()
{
	_upload_path = ConfigDirectiveParser::ParseUploadPath(ctx);
	return !ctx.error.isError();
}

bool	clsLocation::ParseCgiPass()
{
	ConfigDirectiveParser::ParseCGI(ctx, _cgi_pass.empty() ? _cgi_pass : _cgi_pass);
	return !ctx.error.isError();
}

bool	clsLocation::ParseErrorPage()
{
	_error_pages = ConfigDirectiveParser::ParseErrorPage(ctx);
	return !ctx.error.isError();
}



enBlocksDirective
clsLocation::getLocationDirectiveType(const std::string& key)
{
	static std::map<std::string, enBlocksDirective> directives;

	if (directives.empty())
	{
		directives["root"]                  = L_DIR_ROOT;
		directives["index"]                 = L_DIR_INDEX;
		directives["autoindex"]             = L_DIR_AUTOINDEX;
		directives["allow_methods"]        = L_DIR_ACCEPTED_METHODS;
		directives["client_max_body_size"] = L_DIR_CLIENT_MAX_BODY_SIZE;
		directives["return"]                = L_DIR_RETURN;
		directives["upload_path"]          = L_DIR_UPLOAD_PATH;
		directives["cgi_pass"]             = L_DIR_CGI_PASS;
		directives["error_page"]           = L_DIR_ERROR_PAGE;
	}

	std::map<std::string, enBlocksDirective>::iterator it =
		directives.find(key);

	if (it != directives.end())
		return it->second;

	return L_DIR_UNKNOWN;
}

bool    clsLocation::ParseLocationDirective()
{
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (false);

	std::string directive = ctx.parser.peek().value;
	enBlocksDirective dirType = getLocationDirectiveType(directive);

	switch (dirType)
	{
		case L_DIR_ROOT:					return	ParseRoot();
		case L_DIR_INDEX:					return	ParseIndex();
		case L_DIR_AUTOINDEX:				return	ParseAutoIndex();
		case L_DIR_ACCEPTED_METHODS:		return	ParseMethods();
		case L_DIR_CLIENT_MAX_BODY_SIZE:	return	ParseClientMaxBodySize();
		case L_DIR_RETURN:					return	ParseReturn();
		case L_DIR_UPLOAD_PATH:				return	ParseUploadPath();
		case L_DIR_CGI_PASS:				return	ParseCgiPass();
		case L_DIR_ERROR_PAGE:				return	ParseErrorPage();
		default:							return (false);
	}
}

clsLocation::clsLocation(s_parse_context& ctxs)
	: ctx(ctxs)
{
}

bool    clsLocation::parseLocation()
{
	ctx.parser.advance();

	if (!ConfigDirectiveParser::parseLocationPath(ctx, _locationData))
		return false;

	// if (ctx.parser.advance().type != TOKEN_LBRACE)
	// 	return (_ERROR.setStatus(400, "Error in {"), false);

	while (ctx.parser.peek().type != TOKEN_RBRACE &&
		   ctx.parser.peek().type != TOKEN_EOF)
	{
		if (!ParseLocationDirective())
			break;
	}

	if (ctx.parser.peek().type != TOKEN_RBRACE)
		return (ctx.error.setStatus(400, "Error in }"), false);
	ctx.parser.advance();
	while (ctx.parser.peek().type == TOKEN_JOUJNO9ATE)
		ctx.parser.advance();
	return (ctx.error.setStatus(200, ""), true);
}



std::string clsLocation::getRoot() const {
	return _root;
}

std::vector<std::string> clsLocation::getIndex() const {
	return _index;
}

bool clsLocation::getAutoIndex() const {
	return _autoindex;
}

short clsLocation::getAllowMethods() const {
	return _allow_methods;
}

unsigned long long clsLocation::getClientMaxBodySize() const {
	return _client_max_body_size;
}

stReturnData clsLocation::getReturn() const {
	return _return;
}

std::string clsLocation::getUploadPath() const {
	return _upload_path;
}

std::map<std::string, std::string> clsLocation::getCgiPass() const {
	return _cgi_pass;
}

std::map<short, stErrorPagedata> clsLocation::getErrorPages() const {
	return _error_pages;
}

HttpError clsLocation::getError() const {
	return ctx.error;
}

stlocation clsLocation::getLocationData() const {
	return _locationData;
}

