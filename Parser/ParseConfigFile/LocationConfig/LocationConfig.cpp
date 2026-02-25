#include "LocationConfig.hpp"


bool    clsLocation::ParseRoot()
{
    if (_flags & Directives::D_ROOT)
        return !(ctx.error.setStatus(1, "Directives: root already set"), false);
    if (_flags & Directives::D_ALIAS)
        return !(ctx.error.setStatus(1, "Directives: cannot have both root and alias"), false);
    
    _flags |= Directives::D_ROOT;
    _root = ConfigDirectiveParser::ParseRoot(ctx);
    return !ctx.error.isError();
}

bool    clsLocation::ParseAlias()
{
    if (_flags & Directives::D_ALIAS)
        return !(ctx.error.setStatus(1, "Directives: alias already set"), false);
    if (_flags & Directives::D_ROOT)
        return !(ctx.error.setStatus(1, "Directives: cannot have both root and alias"), false);

    _flags |= Directives::D_ALIAS;
    _alias = ConfigDirectiveParser::parseAlias(ctx);
    return !ctx.error.isError();
}

bool    clsLocation::ParseClientMaxBodySize()
{
    if (_flags & Directives::D_MAX_BODY)
        return !(ctx.error.setStatus(1, "Directives: client_max_body_size already set"), false);

    _flags |= Directives::D_MAX_BODY;
    _client_max_body_size = ConfigDirectiveParser::ParseClientMaxBodySize(ctx);
    return !ctx.error.isError();
}

bool    clsLocation::ParseAutoIndex()
{
    if (_flags & Directives::D_AUTOINDEX)
        return !(ctx.error.setStatus(1, "Directives: autoindex already set"), false);

    _flags |= Directives::D_AUTOINDEX;
    _autoindex = ConfigDirectiveParser::ParseAutoIndex(ctx);
    return !ctx.error.isError();
}

bool    clsLocation::ParseReturn()
{
    if (_flags & Directives::D_RETURN)
        return !(ctx.error.setStatus(1, "Directives: return already set"), false);

    _flags |= Directives::D_RETURN;
    _return = ConfigDirectiveParser::ParseReturn(ctx);
    return !ctx.error.isError();
}

bool	clsLocation::ParseIndex()
{
	_index = ConfigDirectiveParser::ParseIndex(ctx);
	return !ctx.error.isError();
}

bool    clsLocation::ParseMethods()
{
    if (_flags & Directives::D_METHODS)
        return !(ctx.error.setStatus(1, "Directives: allow_methods already set"), false);

    _flags |= Directives::D_METHODS;
    _allow_methods = ConfigDirectiveParser::parseMethods(ctx);
    return !ctx.error.isError();
}

bool    clsLocation::ParseUploadStore()
{
    if (_flags & Directives::D_UPLOAD_STORE)
        return !(ctx.error.setStatus(1, "Directives: upload_store already set"), false);

    _flags |= Directives::D_UPLOAD_STORE;
    _upload_store = ConfigDirectiveParser::ParseUploadStore(ctx);
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
		directives["alias"]                  = L_DIR_ALIAS;
		directives["index"]                 = L_DIR_INDEX;
		directives["autoindex"]             = L_DIR_AUTOINDEX;
		directives["allow_methods"]        = L_DIR_ACCEPTED_METHODS;
		directives["client_max_body_size"] = L_DIR_CLIENT_MAX_BODY_SIZE;
		directives["return"]                = L_DIR_RETURN;
		directives["upload_store"]          = L_DIR_UPLOAD_STORE;
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
		case L_DIR_ALIAS:					return	ParseAlias();
		case L_DIR_INDEX:					return	ParseIndex();
		case L_DIR_AUTOINDEX:				return	ParseAutoIndex();
		case L_DIR_ACCEPTED_METHODS:		return	ParseMethods();
		case L_DIR_CLIENT_MAX_BODY_SIZE:	return	ParseClientMaxBodySize();
		case L_DIR_RETURN:					return	ParseReturn();
		case L_DIR_UPLOAD_STORE:			return	ParseUploadStore();
		case L_DIR_CGI_PASS:				return	ParseCgiPass();
		case L_DIR_ERROR_PAGE:				return	ParseErrorPage();
		default:							return (false);
	}
}

clsLocation::clsLocation(s_parse_context& ctxs, const std::string &sRoot
		, const std::vector<std::string>& sIndex,  unsigned long long sClient_max_body_size, bool sAutoIndex)
	: ctx(ctxs)
{
	_root = sRoot;
	_index = sIndex;
	_client_max_body_size = sClient_max_body_size;
	_autoindex = sAutoIndex;
	_flags = Directives::D_NONE;
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
			return (false);
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

const std::vector<std::string> &clsLocation::getIndex() const {
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

std::string clsLocation::getUploadStore() const {
	return _upload_store;
}

const std::map<std::string, std::string> &clsLocation::getCgiPass() const {
	return _cgi_pass;
}

const std::map<short, stErrorPagedata> &clsLocation::getErrorPages() const {
	return _error_pages;
}

const std::string	&clsLocation::getAlias() const
{
	return _alias;
}

HttpError clsLocation::getError() const {
	return ctx.error;
}

stlocation clsLocation::getLocationData() const {
	return _locationData;
}

