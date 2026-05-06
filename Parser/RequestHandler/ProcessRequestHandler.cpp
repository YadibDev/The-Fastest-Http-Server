#include "ProcessRequestHandler.hpp"

ProcessRequestHandler::ProcessRequestHandler()
{
}


static bool	viewEqualsString(const s_view &view, const std::string &str)
{
	if (view.len != str.size())
		return false;
	return (std::memcmp(view.Data, str.data(), view.len) == 0);
}

static bool	viewStartsWithString(const s_view &view, const std::string &prefix)
{
	if (view.len < prefix.size())
		return false;
	return (std::memcmp(view.Data, prefix.data(), prefix.size()) == 0);
}

const clsLocation* ProcessRequestHandler::findBestLocation(
	const std::vector<clsLocation>	&LocationExact,
	const std::vector<clsLocation>	&LocationPrefix,
	const s_view &uri)
{
	const clsLocation* best = NULL;

	for (size_t i = 0; i < LocationExact.size(); i++)
	{
		const std::string& loc = LocationExact[i].getLocationData().uri;
		if (viewEqualsString(uri, loc))
			return &LocationExact[i];
	}

	size_t maxLen = 0;

	for (size_t i = 0; i < LocationPrefix.size(); i++)
	{
		const std::string& loc = LocationPrefix[i].getLocationData().uri;

		if (viewStartsWithString(uri, loc))
		{
			if (loc.size() > maxLen)
			{
				maxLen = loc.size();
				best = &LocationPrefix[i];
			}
		}
	}

	return best;
}

// std::string ProcessRequestHandler::selectMethod(Methods::eMethods method) {
// 	switch (method) {
// 		case Methods::GET:
// 			return "GET";
// 		case Methods::POST:
// 			return "POST";
// 		case Methods::DELETE:
// 			return "DELETE";
// 		default:
// 			return "UNKNOWN";
// 	}
// }

sPathType::e_path_type checkPath(char *path, UriStatus &flags)
{
	if (!path)
		return sPathType::PATH_NOT_FOUND;

	struct stat st;

	if (stat(path, &st) != 0)
		return sPathType::PATH_NOT_FOUND;

	flags.can_read = st.st_mode & (S_IRUSR | S_IRGRP | S_IROTH);
	flags.can_write = st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH);
	flags.can_execute = st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH);

	if (S_ISREG(st.st_mode))
	{
		flags.is_file = true;
		return sPathType::PATH_FILE;
	}
	else if (S_ISDIR(st.st_mode))
	{
		flags.is_dir = true;
		return sPathType::PATH_DIR;
	}

	return (sPathType::PATH_OTHER);
}

bool ProcessRequestHandler::handleDirectory(const clsServerConfig* serverConfig,
											const clsLocation* bestLocation,
											RequestHandler* handler,
											s_uri_entry& newUri,
											HttpError &error)
{
	bool flagType = true;
	const std::vector<s_uri_entry> &vindex = bestLocation->getIndex();
	UriStatus flags;
	sPathType::e_path_type PathType;
	s_uri_entry uri;
	size_t lastIndexOfAutoindex = 0;

	for (size_t i = 0; i < vindex.size(); ++i)
	{
		error.reset();
		uri = vindex[i];
		flagType = uri.flags.is_relative;
		uri.redirect_count = newUri.redirect_count;
		if (flagType)
		{
			if (!createPhysicalPath(bestLocation, handler->getPhysicalPath(), newUri, error))
				continue ;
			lastIndexOfAutoindex = HelperFunctions::ft_strlen(handler->getPhysicalPath());
			HelperFunctions::joinArr(handler->getPhysicalPath(), uri.raw_path.c_str(), MAX_PATH_LEN);
			
			PathType = checkPath(handler->getPhysicalPath(), flags);
			if (PathType == sPathType::PATH_NOT_FOUND || PathType == sPathType::PATH_OTHER)
			{
				error.setStatus(403, "Forbidden");
				continue ;
			}
			uri.raw_path.insert(0, newUri.sv_raw_path.Data, newUri.sv_raw_path.len);
			uri.initView();
		}
		if (!internalRedirect(uri, serverConfig, handler, error))
		{
			if (flagType)
				continue ;
			return false;
		}
		return true;
	}

	if (bestLocation->getAutoIndex())
	{
		handler->setAutoIndex(true);
		handler->getPhysicalPath()[lastIndexOfAutoindex] =  '\0';
		return (error.reset(), true);
	}
	if (error.isError()) // 403, "Forbidden"
		return false;
	return (error.setStatus(403, "Forbidden"), false);
}

size_t percentEncoded(char *buffer, size_t bufferSize, const s_view &uri)
{
	size_t svIdx = 0;
	size_t bufIdx = 0;

	while (svIdx < uri.len && bufIdx < bufferSize - 1) {
		if (uri.Data[svIdx] == '%' && svIdx + 2 < uri.len) {
			buffer[bufIdx++] = HelperFunctions::hexToDecS_view(&uri.Data[svIdx + 1], 2);
			svIdx += 3;
		} else {
			buffer[bufIdx++] = uri.Data[svIdx++];
		}
	}
	buffer[bufIdx] = '\0';
	return bufIdx;
}

bool ProcessRequestHandler::createPhysicalPath(const clsLocation* bestLocation, char *destBuffer, s_uri_entry& newUri, HttpError &error) {

	char    CleanUri[MAX_PATH_LEN];

	const std::string &base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
	size_t currentPos = base.size();

	if (currentPos >= MAX_PATH_LEN - 1)
		return (error.setStatus(414, "URI Too Long"), false);

	memcpy(destBuffer, base.c_str(), currentPos);
	size_t uriPartLen = percentEncoded(CleanUri, MAX_PATH_LEN, newUri.getView());
	const char* uriPart = CleanUri;

	if (!bestLocation->getAlias().empty()) {
		size_t locSize = bestLocation->getLocationData().uri.size();
		if (uriPartLen >= locSize)
		{
			uriPart += locSize;
			uriPartLen -= locSize;
		}
	}

	if (uriPartLen > 0) {

		if (bestLocation->getAlias().empty() && currentPos > 0)
		{
			if (destBuffer[currentPos - 1] != '/' && uriPart[0] != '/')
			{
				if (currentPos < MAX_PATH_LEN - 1)
					destBuffer[currentPos++] = '/';
			}
		}
		if (currentPos > 0 && destBuffer[currentPos - 1] == '/' && uriPart[0] == '/')
		{
			uriPart++;
			uriPartLen--;
		}

		if (currentPos + uriPartLen < MAX_PATH_LEN) {
			memcpy(destBuffer + currentPos, uriPart, uriPartLen);
			currentPos += uriPartLen;
		}
		else
			return (error.setStatus(414, "URI Too Long"), false);
	}

	destBuffer[currentPos] = '\0';

	return true;
}

// void	PostMethod(const clsLocation* bestLocation, RequestHandler* handler, s_uri_entry& newUri)
// {

// }

bool ProcessRequestHandler::isMethodAllowed(HttpTables::eMethod method, uint8_t allowedMethods)
{
	uint8_t methodBit = (1 << method);

	return (methodBit == (allowedMethods & methodBit));
}

bool	ProcessRequestHandler::handleCgi(const clsLocation* bestLocation, RequestHandler* handler, s_uri_entry& newUri, char *PhysicalPath)
{
	HttpError	error;

	handler->ExtractCgiMetadata(newUri, bestLocation->getCgiPass());

	if (handler->getScriptName().len)
	{
		s_uri_entry scriptName;
		scriptName.setSview(handler->getScriptName());
		if (!createPhysicalPath(bestLocation, PhysicalPath,
								scriptName, error))
			return (handler->setError(error), false);
	}
	return true;
}

bool ProcessRequestHandler::validateAndFinalizePhysicalPath(const clsLocation* bestLocation,
															RequestHandler* handler,
															s_uri_entry& newUri,
															HttpError &error)
{
	if (!createPhysicalPath(bestLocation, handler->getPhysicalPath(), newUri, error))
		return false;

	UriStatus flags;
	sPathType::e_path_type PathType;
	memset(&flags, 0, sizeof(flags));

	PathType = checkPath(handler->getPhysicalPath(), flags);

	if (PathType == sPathType::PATH_NOT_FOUND || PathType == sPathType::PATH_OTHER)
		return (error.setStatus(404, "Not Found"), false);

	if (PathType == sPathType::PATH_DIR)
	{
		if ((handler->getMethod() == HttpTables::M_POST))
			return true ;
		stReturnData returnData;
		returnData.code = 301;
		returnData.value.flags.is_dir = true;
		returnData.value.raw_path.insert(0, newUri.sv_raw_path.Data, newUri.sv_raw_path.len);
		returnData.value.raw_path += "/";
		handler->setReturnVal(returnData);
		return true;
	}

	if ((handler->getMethod() == HttpTables::M_GET) && !flags.can_read)
		return (error.setStatus(403, "Forbidden"), false);

	return true;
}

bool ProcessRequestHandler::handlePath(const clsLocation* bestLocation,
										const clsServerConfig* serverConfig,
										RequestHandler* handler,
										s_uri_entry& newUri,
										HttpError &error)
{
	if (newUri.getView().len > 0 && newUri.getView().Data[newUri.getView().len - 1] == '/')
	{
		if (!handleDirectory(serverConfig, bestLocation, handler, newUri, error))
			return false;
		if (handler->getPathInfo().len)
			handler->computePathTranslated(bestLocation->getRoot());
		return true;
	}

	if (!handleCgi(bestLocation, handler, newUri, handler->getPhysicalPath()))
		return false;

	if (handler->getScriptName().len)
	{
		if (handler->getPathInfo().len)
			handler->computePathTranslated(bestLocation->getRoot());
		return true;
	}

	return validateAndFinalizePhysicalPath(bestLocation, handler, newUri, error);
}

void ProcessRequestHandler::finalizeErrorState(RequestHandler* handler, 
											   int originalCode, 
											   const stErrorPagedata& errorData) 
{
	int finalCode = (errorData.response != -1) ? errorData.response : originalCode;

	handler->setStatusError(finalCode);
}

bool ProcessRequestHandler::generateErrorPath(short originalCode,
											const clsServerConfig* serverConfig,
											RequestHandler* handler,
											HttpError &error)
{
	const std::map<short, stErrorPagedata> &ErrorPagedata = serverConfig->getErrorPages();

	std::map<short, stErrorPagedata>::const_iterator it = ErrorPagedata.find(originalCode); // learn this 

	if (it == ErrorPagedata.end())
	{
		finalizeErrorState(handler, originalCode, stErrorPagedata());
		return false;
	}

	if (it->second.uri.flags.is_relative)
	{
		stReturnData returnData;
		returnData.code = it->second.response;
		returnData.value = it->second.uri;
		handler->setReturnVal(returnData);
		return true;
	}

	// copy remove
	stErrorPagedata foundData = it->second;
	foundData.uri.initView();
	if (!internalRedirect(foundData.uri, serverConfig, handler, error))
	{
		originalCode = error.getCodeStatus();	
		finalizeErrorState(handler, originalCode, foundData);
		return false;
	}

	finalizeErrorState(handler, originalCode, foundData);
	return true;
}

bool ProcessRequestHandler::processRequest(const RequestLine& startLine,
										   const clsServerConfig* serverConfig,
										   RequestHandler* handler)
{
	HttpError error;
	s_uri_entry uri;

	const clsLocation* bestLocation = findBestLocation(
		serverConfig->getLocationExact(),
		serverConfig->getLocationPrefix(),
		startLine.getRequestURI().getPath()
	);

	if (!bestLocation)
		return (error.setStatus(500, "Internal Server Error"), false);

	if (!isMethodAllowed(startLine.getMethod(), bestLocation->getAllowMethods()))
	{
		error.setStatus(405, "Method Not Allowed");
		handler->setError(error);
		return false;
	}

	uri.setSview(startLine.getRequestURI().getPath());
	handler->setMethod(startLine.getMethod());
	if ((handler->getMethod() == HttpTables::M_POST))
	{
		if (!handler->ExtractCgiMetadata(uri, bestLocation->getCgiPass()))
		{
			if (bestLocation->getUploadStore().sv_raw_path.len)
			{
				HelperFunctions::join_views(handler->getPhysicalPath(), MAX_PATH_LEN, bestLocation->getUploadStore().sv_raw_path, uri.sv_raw_path);
				return true;
			}
		}		
	}
	if (!internalRedirect(uri, serverConfig, handler, error))
	{
		if (error.isError())
			return (handler->setError(error), false);
		return false;
	}
	handler->setRequestUri(startLine.getRequestURI().getPath());
	handler->setQuery(startLine.getRequestURI().getQuery());
	handler->setVersion(startLine.getVersion());
	return true;
}


bool ProcessRequestHandler::internalRedirect(
	s_uri_entry& newUri,
	const clsServerConfig* serverConfig,
	RequestHandler* handler,
	HttpError& error)
{
	handler->reset();

	if (newUri.redirect_count > INTERNAL_LOOP)
		return (error.setStatus(500, "Internal Server Error"), false);
	newUri.AddRedirectCount();
	const clsLocation* newLocation = findBestLocation(
		serverConfig->getLocationExact(),
		serverConfig->getLocationPrefix(),
		newUri.getView()
	);
	if (!newLocation)
		return (error.setStatus(404, "Not Found"), false);

	handler->setReturn((newLocation->getReturn().code != -1) ? newLocation->getReturn() : serverConfig->getReturn());

	if (handler->getReturn().code != -1)
		return true;

	if (!handlePath(newLocation, serverConfig, handler, newUri, error))
		return false;
	handler->setError(error);

	return true;
}


void	ProcessRequestHandler::convertToAbsUri(s_uri_entry& entry, const s_view& host, const std::string& port)
{

	if (entry.raw_path.empty())
		return ;

	const std::string scheme = "http://";
	
	size_t required = scheme.size() + host.len + 1 + port.size() + entry.raw_path.size();

	std::string absUri;
	absUri.reserve(required);
	absUri.append(scheme);
	absUri.append(host.Data, host.len);

	if (!port.empty()) {
		absUri.push_back(':');
		absUri.append(port);
	}

	absUri.append(entry.raw_path);

	entry.raw_path.swap(absUri);
}


//Repelace newUri with physicalPath
stReturnData	ProcessRequestHandler::buildReturnFromPathAndStatus(s_uri_entry& newUri, short codeStatus, s_view &Host,
															const std::string &Port)
{
	stReturnData returnData;
	returnData.code = codeStatus;

	if (newUri.flags.is_dir || newUri.flags.is_abs_path)
	{
		// creat ABS-URI scheme + Host + Port + Path
		convertToAbsUri(newUri, Host, Port);
	}

	returnData.value = newUri;
	return returnData;
}