#include "ServerConfig.hpp"

clsServerConfig::clsServerConfig(s_parse_context &ctxs) : ctx(ctxs)
{
	_max_body_size = 1048576;

	_root = "default";

	s_uri_entry defaultIndex;
	defaultIndex.raw_path = "index.html";
	defaultIndex.initView();
	_index.push_back(defaultIndex);

	_flags = Directives::D_NONE;
}

clsServerConfig::~clsServerConfig() {}

bool clsServerConfig::ParseRoot()
{
	_root = ConfigDirectiveParser::ParseRoot(ctx);
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool clsServerConfig::ParseIndex()
{
	std::vector<std::string> tmpIndices = ConfigDirectiveParser::ParseIndex(ctx);
	_index.clear();
	for (size_t i = 0; i < tmpIndices.size(); ++i)
	{
		s_uri_entry entry;
		entry.raw_path = tmpIndices[i];
		entry.initView();
		_index.push_back(entry);
	}
	if (ctx.error.isError())
		return (false);
	return (true);
}

bool clsServerConfig::ParseLocation()
{
	clsLocation loc(ctx, _autoindex);
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

// Logic remains identical for the rest
bool clsServerConfig::ParseListen()
{
	_listens.push_back(ConfigDirectiveParser::ParseListen(ctx));
	return !ctx.error.isError();
}

bool clsServerConfig::ParseErrorPage()
{
	ConfigDirectiveParser::ParseErrorPage(ctx, _error_pages);
	return !ctx.error.isError();
}

bool clsServerConfig::ParseClientMaxBodySize()
{
	_max_body_size = ConfigDirectiveParser::ParseClientMaxBodySize(ctx);
	return !ctx.error.isError();
}

bool clsServerConfig::ParseReturn()
{
	if (_flags & Directives::D_RETURN)
		return (ctx.error.setStatus(1, "Directives: return already set"), false);

	_flags |= Directives::D_RETURN;
	_return = ConfigDirectiveParser::ParseReturn(ctx);
	return !ctx.error.isError();
}

bool clsServerConfig::ParseAutoIndex()
{
	_autoindex = ConfigDirectiveParser::ParseAutoIndex(ctx);
	return !ctx.error.isError();
}

enBlocksDirective clsServerConfig::getServerDirectiveType(const std::string &key)
{
	static std::map<std::string, enBlocksDirective> directives;
	if (directives.empty())
	{
		directives["listen"] = L_DIR_LISTEN;
		directives["root"] = L_DIR_ROOT;
		directives["index"] = L_DIR_INDEX;
		directives["client_max_body_size"] = L_DIR_CLIENT_MAX_BODY_SIZE;
		directives["autoindex"] = L_DIR_AUTOINDEX;
		directives["return"] = L_DIR_RETURN;
		directives["error_page"] = L_DIR_ERROR_PAGE;
		directives["location"] = L_DIR_LOCATION;
	}
	std::map<std::string, enBlocksDirective>::iterator it = directives.find(key);
	if (it != directives.end())
		return it->second;
	return L_DIR_UNKNOWN;
}

bool clsServerConfig::ParseServerDirective()
{
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (false);
	std::string directive = ctx.parser.peek().value;
	enBlocksDirective dirType = getServerDirectiveType(directive);

	switch (dirType)
	{
	case L_DIR_LISTEN:
		return ParseListen();
	case L_DIR_ROOT:
		return ParseRoot();
	case L_DIR_INDEX:
		return ParseIndex();
	case L_DIR_CLIENT_MAX_BODY_SIZE:
		return ParseClientMaxBodySize();
	case L_DIR_AUTOINDEX:
		return ParseAutoIndex();
	case L_DIR_RETURN:
		return ParseReturn();
	case L_DIR_ERROR_PAGE:
		return ParseErrorPage();
	case L_DIR_LOCATION:
		return ParseLocation();
	default:
		return (ctx.error.setStatus(400, "Error in " + directive), false);
	}
}

void clsServerConfig::initUri()
{
	ConfigDirectiveParser::DefineUri(_return.value);
	_return.value.initView();

	for (size_t i = 0; i < _LocationExact.size(); i++)
	{
		if (_LocationExact[i].getRoot().empty() && _LocationExact[i].getAlias().empty())
			_LocationExact[i].setRoot(_root);
		if (!_LocationExact[i].getErrorPages().size())
			_LocationExact[i].setErrorPages(this->_error_pages);
		if (!_LocationExact[i].getIndex().size())
			_LocationExact[i].setIndex(this->_index);
		_LocationExact[i].initUri();
	}

	for (size_t i = 0; i < _LocationPrefix.size(); i++)
	{
		if (_LocationPrefix[i].getRoot().empty() && _LocationPrefix[i].getAlias().empty())
			_LocationPrefix[i].setRoot(_root);
		if (!_LocationPrefix[i].getErrorPages().size())
			_LocationPrefix[i].setErrorPages(this->_error_pages);
		if (!_LocationPrefix[i].getIndex().size())
			_LocationPrefix[i].setIndex(this->_index);
		_LocationPrefix[i].initUri();
	}

	size_t i = 0;
	while (i < _index.size())
	{
		ConfigDirectiveParser::DefineUri(_index[i]);
		_index[i].initView();
		i++;
	}

	std::map<short, stErrorPagedata>::iterator it_err;
	for (it_err = _error_pages.begin(); it_err != _error_pages.end(); ++it_err)
	{
		ConfigDirectiveParser::DefineUri(it_err->second.uri);
		it_err->second.uri.initView();
	}

	ConfigDirectiveParser::DefineUri(_return.value);
	_return.value.initView();
	ConfigDirectiveParser::DefineUri(_return.value);
	_return.value.initView();
}

bool clsServerConfig::parseBlockServer()
{
	ctx.parser.advance();
	ConfigDirectiveParser::skipWhitespace(ctx.parser);
	if (ctx.parser.peek().type != TOKEN_LBRACE)
		return (ctx.error.setStatus(400, "Syntax Error: Expected '{'"), false);

	ctx.parser.advance();
	ConfigDirectiveParser::skipWhitespace(ctx.parser);

	while (ctx.parser.peek().type != TOKEN_RBRACE && ctx.parser.peek().type != TOKEN_EOF)
	{
		if (!ParseServerDirective())
			return (false);
	}

	if (ctx.parser.peek().type != TOKEN_RBRACE)
		return (ctx.error.setStatus(400, "Syntax Error: Expected '}'"), false);

	ctx.parser.advance();
	ConfigDirectiveParser::skipWhitespace(ctx.parser);
	return true;
}

// Getters updated for s_uri_entry
const std::string &clsServerConfig::getRoot() const { return _root; }
const std::vector<s_uri_entry> &clsServerConfig::getIndex() const { return _index; }

// Other Getters
std::vector<sockaddr_in> clsServerConfig::getListens() const { return _listens; }
const std::map<short, stErrorPagedata> &clsServerConfig::getErrorPages() const { return _error_pages; }
size_t clsServerConfig::getMaxBodySize() const { return _max_body_size; }
const std::vector<clsLocation> &clsServerConfig::getLocationExact() const { return _LocationExact; }
const std::vector<clsLocation> &clsServerConfig::getLocationPrefix() const { return _LocationPrefix; }
const stReturnData &clsServerConfig::getReturn() const { return _return; }
bool clsServerConfig::getAutoIndex() const { return _autoindex; }
HttpError clsServerConfig::getError() const { return ctx.error; }