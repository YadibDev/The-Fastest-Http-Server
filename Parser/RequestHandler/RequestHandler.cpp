#include "RequestHandler.hpp"
#include <cstring> 

RequestHandler::RequestHandler(stPollRequest& request)
	:	_Header(request),
		_pathCgi(NULL),
		_upload_store(NULL)
{
	_allowMethods = 0;
	_method = HttpTables::M_UNKNOWN;
	_autoindex = false;
	_physicalPath[0] = '\0';
	_query.Data = NULL;
	_query.len = 0;
	_version.Data = NULL;
	_version.len = 0;
}

RequestHandler::~RequestHandler() {}

bool	RequestHandler::ExtractScriptName(const s_view &uri, std::string &extensionStr)
{
	s_view	extView;
	s_view	Start = uri;
	std::map<std::string, std::string>::const_iterator it;

	do
	{
		extView = HelperFunctions::find_last_of_view(uri, ".");
		Start.len = extView.len;
		it = cgi_pass.find(extensionStr);

		if (it != cgi_pass.end())
		{
			_pathCgi = &it->second;
			return true;
		}			 

	} while (extView.Data);

	// memcpy(&extensionStr[0], extView.Data, copyLen);

}

bool	RequestHandler::HandlerCgi(const s_view &uri, const std::map<std::string, std::string> &cgi_pass)
{
	// _PathInfo = HelperFunctions::find_first_of_view(extView, "/");
	
	// if (!extView.Data || !extView.len || extView.len > 255)
	// 	return true;

	

	// size_t copyLen = extView.len - _PathInfo.len;
	// if (copyLen > 255)
	// 	return (_error.setStatus(414, "URI Too Long"), false);
	std::string extensionStr;

	ExtractScriptName(uri, extensionStr);

	std::map<std::string, std::string>::const_iterator it = cgi_pass.find(extensionStr);

	if (it != cgi_pass.end())
		_pathCgi = &it->second;
	return true;	
}

void	RequestHandler::computePathTranslated(const std::string& rootPath)
{
	size_t totalSize = rootPath.size() + _PathInfo.len;

	bool needSlash = (!rootPath.empty() && rootPath[rootPath.size() - 1] != '/' && _PathInfo.len > 0 && _PathInfo.Data[0] != '/');

	if (needSlash)
		totalSize += 1;

	_PathTranslated.reserve(totalSize);
	_PathTranslated.append(rootPath);

	if (needSlash)
		_PathTranslated += '/';

	_PathTranslated.append(_PathInfo.Data, _PathInfo.len);
}

void    RequestHandler::setAutoIndex(bool autoindex) { _autoindex = autoindex; }

void    RequestHandler::setQuery(const s_view query) { _query = query; }

void    RequestHandler::setVersion(const s_view version) { _version = version; }

void    RequestHandler::setMethod(HttpTables::eMethod method) { _method = method; }

void    RequestHandler::setAllowedMethods(uint8_t allowed) { _allowMethods = allowed; }

void    RequestHandler::setErrorPages(const std::map<short, stErrorPagedata>& errorPages)
{
	_error_pages = errorPages;
}

void	RequestHandler::setDefaultErrorPage(const stErrorPagedata* defaultErrorPage)
{
	_defaultErrorPage = defaultErrorPage;
}

void RequestHandler::setPathTranslated(std::string pathTranslated) {
	_PathTranslated = pathTranslated;
}

void RequestHandler::setServerPort(const s_view serverPort) {
	_ServerPort = serverPort;
}

void    RequestHandler::setHeader(HeaderTable	Header)
{
	_Header = Header;
}

void    RequestHandler::setPathCgi(const std::string* pathCgi) { _pathCgi = pathCgi; }

void    RequestHandler::setReturn(const stReturnData& returnData) { _return = returnData; }

void    RequestHandler::setUploadStore(const std::string* uploadStore) { _upload_store = uploadStore; }

void    RequestHandler::setBody(const std::string& body) { _body = body; }

void    RequestHandler::setFilePathBody(const std::string& filePathBody) { _filePathBody = filePathBody; }

void    RequestHandler::setError(const HttpError &error) { _error = error; }

char* RequestHandler::getPhysicalPath() { return _physicalPath; }

bool RequestHandler::getAutoIndex() const { return _autoindex; }

s_view RequestHandler::getQuery() const { return _query; }

s_view RequestHandler::getVersion() const { return _version; }

const s_view& RequestHandler::getPathInfo() const {
	return _PathInfo;
}

const std::string &RequestHandler::getPathTranslated() const {
	return _PathTranslated;
}

const s_view& RequestHandler::getServerPort() const {
	return _ServerPort;
}

HttpTables::eMethod RequestHandler::getMethod() const { return _method; }


HeaderTable    &RequestHandler::getHeader()
{
	return _Header;
}

const stErrorPagedata *RequestHandler::getErrorPage(short code) const {
	std::map<short, stErrorPagedata>::const_iterator it = _error_pages.find(code);
	if (it == _error_pages.end())
		return _defaultErrorPage;
	return &it->second;
}

const std::string* RequestHandler::getPathCgi() const { return _pathCgi; }

const stReturnData& RequestHandler::getReturn() const { return _return; }

const std::string* RequestHandler::getUploadStore() const { return _upload_store; }

const std::string& RequestHandler::getBody() const { return _body; }

const std::string& RequestHandler::getFilePathBody() const { return _filePathBody; }

const HttpError& RequestHandler::getError() const { return _error; }