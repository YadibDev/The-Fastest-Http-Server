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
	_PathTranslated.resize(4096);
}

void RequestHandler::reset()
{
	_physicalPath[0] = '\0';

	_autoindex = false;

	_query.reset();
	_version.reset();
	_PathInfo.reset();
	_ScriptName.reset();

	_PathTranslated.clear();
	_filePathBody.clear();

	_method = HttpTables::M_GET;
	_allowMethods = 0;


	_defaultErrorPage = false;
	_pathCgi = NULL;
	_upload_store = NULL;
	_statusError = 0; // adib add this
	_error = HttpError();
}

RequestHandler::~RequestHandler() {}

bool	RequestHandler::ExtractCgiMetadata(s_uri_entry& newUri, const std::map<std::string, std::string> &cgi_pass)
{
	if (!newUri.getView().Data || newUri.getView().len == 0)
		return false;

	char* start = newUri.getView().Data;
	char* current = start;
	char* end = start + newUri.getView().len - 1;

	while (current <= end)
	{
		if (*current == '.')
		{
			char* extStart = current;
			char* extEnd = extStart;

			while (extEnd <= end && *extEnd != '/')
				++extEnd;

			std::string extStr(extStart, extEnd);

			std::map<std::string, std::string>::const_iterator it = cgi_pass.find(extStr);
			if (it != cgi_pass.end())
			{
				_pathCgi = &it->second;

				_ScriptName.Data = start;
				_ScriptName.len = static_cast<uint16_t>(extEnd - start);

				if (extEnd <= end && *extEnd == '/')
				{
					_PathInfo.Data = extEnd;
					_PathInfo.len = static_cast<uint16_t>(end - extEnd + 1);
				}
				else
				{
					_PathInfo.Data = NULL;
					_PathInfo.len = 0;
				}
				return true;
			}
		}
		++current;
	}
	return false;
}


void	RequestHandler::computePathTranslated(const std::string& rootPath, const clsServerConfig* serverConfig)
{

	bool	isExist = false;
	const clsLocation* newLocation = ProcessRequestHandler::findBestLocation(
		serverConfig->getLocationExact(),
		serverConfig->getLocationPrefix(),
		_PathInfo
	);
	s_uri_entry newUri;

	if (newLocation)
	{
		if (PathResolver::createPhysicalPath(newLocation, &_PathTranslated[0], _PathInfo, _error))
			isExist = true;
	}

	if (!isExist)
	{
		size_t totalSize = rootPath.size() + _PathInfo.len;

		bool needSlash = (!rootPath.empty() && rootPath[rootPath.size() - 1] != '/' && _PathInfo.len > 0 && _PathInfo.Data[0] != '/');

		if (needSlash)
			totalSize += 1;

		_PathTranslated.append(rootPath);

		if (needSlash)
			_PathTranslated += '/';

		_PathTranslated.append(_PathInfo.Data, _PathInfo.len);
	}
}

void	RequestHandler::setRequestUri(const s_view &uri)
{
	_requestUri = uri;
}

void	RequestHandler::setSizeFile(size_t sizeFile)
{
	_sizeFile = sizeFile;
}

void    RequestHandler::setAutoIndex(bool autoindex) { _autoindex = autoindex; }

void    RequestHandler::setQuery(const s_view query) { _query = query; }

void    RequestHandler::setVersion(const s_view version) { _version = version; }

void    RequestHandler::setMethod(HttpTables::eMethod method) { _method = method; }

void    RequestHandler::setAllowedMethods(uint8_t allowed) { _allowMethods = allowed; }


void	RequestHandler::setDefaultErrorPage(bool defaultErrorPage)
{
	_defaultErrorPage = defaultErrorPage;
}

void RequestHandler::setPathTranslated(std::string pathTranslated) {
	_PathTranslated = pathTranslated;
}


void    RequestHandler::setHeader(HeaderTable	Header)
{
	_Header = Header;
}

void    RequestHandler::setPathCgi(const std::string* pathCgi) { _pathCgi = pathCgi; }

void    RequestHandler::setReturn(const stReturnData &returnData) { _return = returnData; }

void	RequestHandler::setReturnVal(stReturnData returnData) { _return = returnData; }

void    RequestHandler::setUploadStore(const std::string* uploadStore) { _upload_store = uploadStore; }


void    RequestHandler::setFilePathBody(const std::string& filePathBody) { _filePathBody = filePathBody; }

void	RequestHandler::setStatusError(short statusError) { _statusError = statusError; }

void    RequestHandler::setError(const HttpError &error) { _error = error; }

// edited by achraf i add const 
// char* RequestHandler::getPhysicalPath() const { return _physicalPath; }

size_t	RequestHandler::getSizeFile() const
{
	return _sizeFile;
}

const char* RequestHandler::getPhysicalPath() const{ return _physicalPath; }
char* RequestHandler::getPhysicalPath() { return _physicalPath; }

bool RequestHandler::getAutoIndex() const { return _autoindex; }

const s_view &RequestHandler::getQuery() const { return _query; }

const s_view &RequestHandler::getVersion() const { return _version; }

const s_view& RequestHandler::getPathInfo() const {
	return _PathInfo;
}

const std::string &RequestHandler::getPathTranslated() const {
	return _PathTranslated;
}


HttpTables::eMethod RequestHandler::getMethod() const { return _method; }

const s_view			&RequestHandler::getRequestUri() const
{
	return _requestUri;
}

const HeaderTable    &RequestHandler::getHeader() const
{
	return _Header;
}

const s_view	&RequestHandler::getScriptName() const
{
	return _ScriptName;
}

bool	RequestHandler::getDefaultErrorPage() const { return _defaultErrorPage; }


const std::string* RequestHandler::getPathCgi() const { return _pathCgi; }

const stReturnData& RequestHandler::getReturn() const { return _return; }

const std::string* RequestHandler::getUploadStore() const { return _upload_store; }


const std::string& RequestHandler::getFilePathBody() const { return _filePathBody; }

short				RequestHandler::getStatusError() { return _statusError; }

const HttpError& RequestHandler::getError() const { return _error; }