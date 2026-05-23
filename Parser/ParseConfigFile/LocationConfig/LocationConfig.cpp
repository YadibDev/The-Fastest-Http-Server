#include "LocationConfig.hpp"

bool    clsLocation::ParseRoot()
{
	if (_flags & Directives::D_ROOT)
		return (ctx.error.setStatus(1, "Directives: root already set"), false);
	if (_flags & Directives::D_ALIAS)
		return (ctx.error.setStatus(1, "Directives: cannot have both root and alias"), false);
	
	_flags |= Directives::D_ROOT;
	_root = ConfigDirectiveParser::ParseRoot(ctx);
	return !ctx.error.isError();
}

bool    clsLocation::ParseAlias()
{
	if (_flags & Directives::D_ALIAS)
		return (ctx.error.setStatus(1, "Directives: alias already set"), false);
	if (_flags & Directives::D_ROOT)
		return (ctx.error.setStatus(1, "Directives: cannot have both root and alias"), false);

	_flags |= Directives::D_ALIAS;
	_alias = ConfigDirectiveParser::parseAlias(ctx);
	return !ctx.error.isError();
}

bool    clsLocation::ParseIndex()
{
	std::vector<std::string> tmpIndices = ConfigDirectiveParser::ParseIndex(ctx);
	s_uri_entry defaultIndex;

	_index.clear();

	for (size_t i = 0; i < tmpIndices.size(); ++i) {
		s_uri_entry entry;
		entry.raw_path = tmpIndices[i];
		_index.push_back(entry);
		_index[i].initView();
	}
	return !ctx.error.isError();
}

bool    clsLocation::ParseUploadStore()
{
	if (_flags & Directives::D_UPLOAD_STORE)
		return (ctx.error.setStatus(1, "Directives: upload_store already set"), false);

	_flags |= Directives::D_UPLOAD_STORE;
	_upload_store.raw_path = ConfigDirectiveParser::ParseUploadStore(ctx);
	_upload_store.initView();
	return !ctx.error.isError();
}

bool    clsLocation::ParseUploadLocation()
{
	if (_flags & Directives::D_UPLOAD_LOCATION)
		return (ctx.error.setStatus(1, "Directives: upload_location already set"), false);

	_flags |= Directives::D_UPLOAD_LOCATION;
	_upload_location = ConfigDirectiveParser::ParseUploadLocation(ctx);
	return !ctx.error.isError();
}

bool    clsLocation::ParseCgiPass()
{
	ConfigDirectiveParser::ParseCGI(ctx, _cgi_pass);
	
	return !ctx.error.isError();
}

// Remaining Parse functions kept logic-identical, only using changed types
bool    clsLocation::ParseClientMaxBodySize() {
	if (_flags & Directives::D_MAX_BODY)
		return (ctx.error.setStatus(1, "Directives: client_max_body_size already set"), false);
	_flags |= Directives::D_MAX_BODY;
	_client_max_body_size = ConfigDirectiveParser::ParseClientMaxBodySize(ctx);
	return !ctx.error.isError();
}

bool    clsLocation::ParseAutoIndex() {
	if (_flags & Directives::D_AUTOINDEX)
		return (ctx.error.setStatus(1, "Directives: autoindex already set"), false);
	_flags |= Directives::D_AUTOINDEX;
	_autoindex = ConfigDirectiveParser::ParseAutoIndex(ctx);
	return !ctx.error.isError();
}

bool    clsLocation::ParseReturn() {
	if (_flags & Directives::D_RETURN)
		return (ctx.error.setStatus(1, "Directives: return already set"), false);
	_flags |= Directives::D_RETURN;
	_return = ConfigDirectiveParser::ParseReturn(ctx);
	return !ctx.error.isError();
}

bool    clsLocation::ParseMethods() {
	if (_flags & Directives::D_METHODS)
		return (ctx.error.setStatus(1, "Directives: allow_methods already set"), false);
	_flags |= Directives::D_METHODS;
	_allow_methods = ConfigDirectiveParser::parseMethods(ctx);
	return !ctx.error.isError();
}

bool    clsLocation::ParseErrorPage() {
	ConfigDirectiveParser::ParseErrorPage(ctx, _error_pages);
	return !ctx.error.isError();
}

clsLocation::clsLocation(s_parse_context& ctxs, bool sAutoIndex)
	: ctx(ctxs)
{
	_allow_methods = 8;
	_autoindex = sAutoIndex;
	_flags = Directives::D_NONE;
}

clsLocation::clsLocation(const clsLocation &loc) : ctx(loc.ctx)
{
	_alias = loc._alias;
	_flags = loc._flags;
	_locationData = loc._locationData;
	_root = loc._root;
	_alias = loc._alias;
	_index = loc._index;
	_autoindex = loc._autoindex;
	_allow_methods = loc._allow_methods;
	_client_max_body_size = loc._client_max_body_size;
	_return = loc._return;
	_upload_store = loc._upload_store;
	_upload_location = loc._upload_location;
	_cgi_pass = loc._cgi_pass;
	_error_pages = loc._error_pages;
	initUri();
}

const std::string &clsLocation::getRoot() const { return _root; }
const std::string &clsLocation::getAlias() const { return _alias; }
const std::vector<s_uri_entry> &clsLocation::getIndex() const { return _index; }
const s_uri_entry &clsLocation::getUploadStore() const { return _upload_store; }
const std::string	&clsLocation::getUploadLocation() const {return _upload_location;}
const std::map<std::string, std::string> &clsLocation::getCgiPass() const { return _cgi_pass; }

const stReturnData &clsLocation::getReturn() const { return _return; }
bool clsLocation::getAutoIndex() const { return _autoindex; }
short clsLocation::getAllowMethods() const { return _allow_methods; }
unsigned long long clsLocation::getClientMaxBodySize() const { return _client_max_body_size; }
const std::map<short, stErrorPagedata> &clsLocation::getErrorPages() const { return _error_pages; }
HttpError clsLocation::getError() const { return ctx.error; }
const stlocation &clsLocation::getLocationData() const { return _locationData; }


void	clsLocation::setRoot(const std::string& root) { _root = root; }
void	clsLocation::setAlias(const std::string& alias) { _alias = alias; }
void	clsLocation::setIndex(const std::vector<s_uri_entry>& index) { _index = index; }
void	clsLocation::setAutoindex(bool autoindex) { _autoindex = autoindex; }
void	clsLocation::setAllowMethods(short methods) { _allow_methods = methods; }
void	clsLocation::setClientMaxBodySize(unsigned long long size) { _client_max_body_size = size; }
void	clsLocation::setReturn(const stReturnData& ret) { _return = ret; }
void	clsLocation::setUploadStore(const s_uri_entry& store) { _upload_store = store; }
void	clsLocation::setUploadLocation(std::string store) { _upload_location = store; }
void	clsLocation::setCgiPass(const std::map<std::string, std::string>& cgi) { _cgi_pass = cgi; }
void	clsLocation::setErrorPages(const std::map<short, stErrorPagedata>& pages) { _error_pages = pages; }


void clsLocation::initUri()
{
    ConfigDirectiveParser::DefineUri(_return.value);
    _return.value.initView();

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

    ConfigDirectiveParser::DefineUri(_upload_store);
    _upload_store.initView();
}

enBlocksDirective clsLocation::getLocationDirectiveType(const std::string& key) {
	static std::map<std::string, enBlocksDirective> directives;
	if (directives.empty()) {
		directives["root"] = L_DIR_ROOT;
		directives["alias"] = L_DIR_ALIAS;
		directives["index"] = L_DIR_INDEX;
		directives["autoindex"] = L_DIR_AUTOINDEX;
		directives["allow_methods"] = L_DIR_ACCEPTED_METHODS;
		directives["client_max_body_size"] = L_DIR_CLIENT_MAX_BODY_SIZE;
		directives["return"] = L_DIR_RETURN;
		directives["upload_store"] = L_DIR_UPLOAD_STORE;
		directives["upload_location"] = L_DIR_UPLOAD_LOCATION;
		directives["cgi_pass"] = L_DIR_CGI_PASS;
		directives["error_page"] = L_DIR_ERROR_PAGE;
	}
	std::map<std::string, enBlocksDirective>::iterator it = directives.find(key);
	if (it != directives.end()) return it->second;
	return L_DIR_UNKNOWN;
}

bool clsLocation::ParseLocationDirective(s_parse_context &ctx) {
	if (ctx.parser.peek().type != TOKEN_WORD) return (false);
	std::string directive = ctx.parser.peek().value;
	enBlocksDirective dirType = getLocationDirectiveType(directive);
	switch (dirType) {
		case L_DIR_ROOT: return ParseRoot();
		case L_DIR_ALIAS: return ParseAlias();
		case L_DIR_INDEX: return ParseIndex();
		case L_DIR_AUTOINDEX: return ParseAutoIndex();
		case L_DIR_ACCEPTED_METHODS: return ParseMethods();
		case L_DIR_CLIENT_MAX_BODY_SIZE: return ParseClientMaxBodySize();
		case L_DIR_RETURN: return ParseReturn();
		case L_DIR_UPLOAD_STORE: return ParseUploadStore();
		case L_DIR_UPLOAD_LOCATION: return ParseUploadLocation();
		case L_DIR_CGI_PASS: return ParseCgiPass();
		case L_DIR_ERROR_PAGE: return ParseErrorPage();
		default: return (ctx.error.setStatus(400, "Error in " + directive), false);
	}
}

bool	clsLocation::checkUploadStoreAndUploadLocation(HttpError &error)
{
	if (_flags & Directives::D_UPLOAD_STORE)
	{
		if (_flags & Directives::D_UPLOAD_LOCATION)
			return (true);
		return (error.setStatus(400, "Upload_Location Is Not Found"), false);
	}
	return true;
}

bool clsLocation::parseLocation() {
	ctx.parser.advance();
	if (!ConfigDirectiveParser::parseLocationPath(ctx, _locationData))
		return false;
	while (ctx.parser.peek().type != TOKEN_RBRACE && ctx.parser.peek().type != TOKEN_EOF)
		if (!ParseLocationDirective(ctx))
			return (false);
	if (ctx.parser.peek().type != TOKEN_RBRACE)
		return (ctx.error.setStatus(400, "Error in }"), false);
	ctx.parser.advance();
	while (ctx.parser.peek().type == TOKEN_NEW_LINE)
		ctx.parser.advance();
	initUri();
	return (checkUploadStoreAndUploadLocation(ctx.error));
}