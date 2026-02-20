#include "RequestHandler.hpp"

RequestHandler::RequestHandler()
    : _physicalPath(""),
      _autoindex(false),
      _allowMethod(0),
      _method(GET),
      _upload_store("") {
}

RequestHandler::~RequestHandler() {
}

void RequestHandler::setPhysicalPath(const std::string& path) {
    _physicalPath = path;
}

void RequestHandler::setAutoIndex(bool autoindex) {
    _autoindex = autoindex;
}

void RequestHandler::setAllowMethod(bool allowMethod) {
    _allowMethod = allowMethod;
}

void RequestHandler::setQuery(const std::string& query) {
    _query = query;
}

void RequestHandler::setMethod(eMethods method) {
    _method = method;
}

void RequestHandler::setHeaders(const std::map<std::string, std::vector<std::string> >& headers) {
    _headers = headers;
}

void RequestHandler::setErrorPages(const std::map<short, stErrorPagedata>& errorPages) {
    _error_pages = errorPages;
}

void RequestHandler::setPathCgi(const std::string& pathCgi) {
    _pathCgi = pathCgi;
}

void RequestHandler::setReturn(stReturnData returnData) {
    _return = returnData;
}

void RequestHandler::setUploadStore(const std::string& uploadStore) {
    _upload_store = uploadStore;
}

void RequestHandler::setBody(const std::string& body) {
    _body = body;
}

void RequestHandler::setFilePathBody(const std::string& filePathBody) {
    _filePathBody = filePathBody;
}

const std::string& RequestHandler::getPhysicalPath() const {
    return _physicalPath;
}

bool RequestHandler::getAutoIndex() const {
    return _autoindex;
}

bool RequestHandler::getAllowMethod() const {
    return _allowMethod;
}

const std::string& RequestHandler::getQuery() const {
    return _query;
}

eMethods RequestHandler::getMethod() const {
    return _method;
}

std::vector<std::string> RequestHandler::getHeaderValues(const std::string& key) const
{
    std::map<std::string, std::vector<std::string> >::const_iterator it = _headers.find(key);

    if (it == _headers.end())
        return std::vector<std::string>();

    return it->second;
}

stErrorPagedata RequestHandler::getErrorPage(short code) const
{

    std::map<short, stErrorPagedata>::const_iterator it = _error_pages.find(code);

    if (it == _error_pages.end())
        return stErrorPagedata();

    return it->second;
}

const std::string& RequestHandler::getPathCgi() const {
    return _pathCgi;
}

const stReturnData& RequestHandler::getReturn() const {
    return _return;
}

const std::string& RequestHandler::getUploadStore() const {
    return _upload_store;
}

const std::string& RequestHandler::getBody() const {
    return _body;
}

const std::string& RequestHandler::getFilePathBody() const {
    return _filePathBody;
}

const stReturnData& RequestHandler::getReturnData() const {
    return _return;
}

