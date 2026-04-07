#include "RequestHandler.hpp"
#include <cstring> 

RequestHandler::RequestHandler(stPollRequest& request)
    : _autoindex(false),
      _method(HttpTables::M_UNKNOWN),
      _allowMethods(0),
      _pathCgi(NULL),
      _upload_store(NULL),
      _Header(request)
{
    _physicalPath[0] = '\0';
    _query.Data = NULL;
    _query.len = 0;
    _version.Data = NULL;
    _version.len = 0;
}

RequestHandler::~RequestHandler() {}


void    RequestHandler::setAutoIndex(bool autoindex) { _autoindex = autoindex; }

void    RequestHandler::setQuery(const s_view query) { _query = query; }

void    RequestHandler::setVersion(const s_view version) { _version = version; }

void    RequestHandler::setMethod(HttpTables::eMethod method) { _method = method; }

void    RequestHandler::setAllowedMethods(uint8_t allowed) { _allowMethods = allowed; }

const void    RequestHandler::setErrorPages(const std::map<short, stErrorPagedata>& errorPages)
{
    _error_pages = errorPages;
}

void RequestHandler::setPathInfo(const s_view pathInfo) {
    _PathInfo = pathInfo;
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

stErrorPagedata RequestHandler::getErrorPage(short code) const {
    std::map<short, stErrorPagedata>::const_iterator it = _error_pages.find(code);
    if (it == _error_pages.end())
        return stErrorPagedata();
    return it->second;
}

const std::string* RequestHandler::getPathCgi() const { return _pathCgi; }

const stReturnData& RequestHandler::getReturn() const { return _return; }

const std::string* RequestHandler::getUploadStore() const { return _upload_store; }

const std::string& RequestHandler::getBody() const { return _body; }

const std::string& RequestHandler::getFilePathBody() const { return _filePathBody; }

const HttpError& RequestHandler::getError() const { return _error; }