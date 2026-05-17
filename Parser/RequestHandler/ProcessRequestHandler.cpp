#include "ProcessRequestHandler.hpp"

ProcessRequestHandler::ProcessRequestHandler() {}

const clsLocation* ProcessRequestHandler::findBestLocation(
	const std::vector<clsLocation> &LocationExact,
	const std::vector<clsLocation> &LocationPrefix,
	const s_view &uri)
{
	for (size_t i = 0; i < LocationExact.size(); i++) {
		if (PathResolver::viewEqualsString(uri, LocationExact[i].getLocationData().uri))
			return &LocationExact[i];
	}
	const clsLocation* best = NULL;
	size_t maxLen = 0;
	for (size_t i = 0; i < LocationPrefix.size(); i++) {
		const std::string& loc = LocationPrefix[i].getLocationData().uri;
		if (PathResolver::viewStartsWithString(uri, loc)) {
			if (loc.size() > maxLen) {
				maxLen = loc.size();
				best = &LocationPrefix[i];
			}
		}
	}
	return best;
}

bool ProcessRequestHandler::handleDirectory(const clsServerConfig* serverConfig, const clsLocation* bestLocation, RequestHandler* handler, s_uri_entry& newUri, HttpError &error) {
	const std::vector<s_uri_entry> &vindex = bestLocation->getIndex();
	UriStatus flags;
	size_t lastIndexOfAutoindex = 0;

	for (size_t i = 0; i < vindex.size(); ++i) {
		error.reset();
		s_uri_entry uri;
		uri.sv_raw_path = vindex[i].sv_raw_path;
		bool isRelativePath = vindex[i].flags.is_relative;
		uri.redirect_count = newUri.redirect_count;

		if (isRelativePath) {
			if (!PathResolver::createPhysicalPath(bestLocation, handler->getPhysicalPath(), newUri.getView(), error))
				continue;
			lastIndexOfAutoindex = HelperFunctions::ft_strlen(handler->getPhysicalPath());
			HelperFunctions::joinArr(handler->getPhysicalPath(), uri.sv_raw_path.Data, lastIndexOfAutoindex, uri.sv_raw_path.len, MAX_PATH_LEN);
			size_t size = 0;
			if (PathResolver::checkPath(handler->getPhysicalPath(), flags, size) != sPathType::PATH_FILE) {
				error.setStatus(403, "Forbidden");
				continue;
			}
			uri.raw_path.insert(0, newUri.sv_raw_path.Data, newUri.sv_raw_path.len);
			uri.raw_path.insert(newUri.sv_raw_path.len, uri.sv_raw_path.Data, uri.sv_raw_path.len);
			uri.initView();
		}
		if (!internalRedirect(uri, serverConfig, handler, error)) {
			if (isRelativePath) continue;
			return false;
		}
		return true;
	}
	if (bestLocation->getAutoIndex()) {
		handler->setAutoIndex(true);
		handler->getPhysicalPath()[lastIndexOfAutoindex] = '\0';
		return (error.reset(), true);
	}
	return (error.setStatus(403, "Forbidden"), false);
}

bool ProcessRequestHandler::isMethodAllowed(HttpTables::eMethod method, uint8_t allowedMethods) {
	return ((1 << method) & allowedMethods);
}

bool ProcessRequestHandler::handleCgi(const clsLocation* bestLocation, 
                                      RequestHandler* handler, 
                                      s_uri_entry& newUri, 
                                      char *PhysicalPath, 
                                      HttpError &error) 
{
    handler->ExtractCgiMetadata(newUri, bestLocation->getCgiPass());
    if (handler->getScriptName().len) {
        s_uri_entry scriptName;
        scriptName.setSview(handler->getScriptName());
        
        if (!PathResolver::createPhysicalPath(bestLocation, PhysicalPath, scriptName.getView(), error))
            return false;

        UriStatus flags;
        size_t size = 0;
        sPathType::e_path_type PathType = PathResolver::checkPath(PhysicalPath, flags, size);

        if (PathType == sPathType::PATH_NOT_FOUND || PathType == sPathType::PATH_OTHER) {
            error.setStatus(404, "Not Found");
            return false;
        }
        if (PathType == sPathType::PATH_DIR) {
            error.setStatus(403, "Forbidden");
            return false;
        }
    }
    return true;
}

bool ProcessRequestHandler::validateAndFinalizePhysicalPath(const clsLocation* bestLocation, 
                                                            RequestHandler* handler, 
                                                            s_uri_entry& newUri, 
                                                            HttpError &error) 
{
    if (!PathResolver::createPhysicalPath(bestLocation, handler->getPhysicalPath(), newUri.getView(), error))
        return false;
        
    UriStatus flags;
    size_t size = 0;
    sPathType::e_path_type PathType = PathResolver::checkPath(handler->getPhysicalPath(), flags, size);
    handler->setSizeFile(size);

    if (PathType == sPathType::PATH_NOT_FOUND || PathType == sPathType::PATH_OTHER)
        return (error.setStatus(404, "Not Found"), false);

    if (PathType == sPathType::PATH_DIR) {
        stReturnData returnData;
        returnData.code = 301;
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
			handler->computePathTranslated(bestLocation->getRoot(), serverConfig);
		return true;
	}

    if (!handleCgi(bestLocation, handler, newUri, handler->getPhysicalPath(), error))
	{
        return false;
	}

	if (handler->getScriptName().len)
	{
		if (handler->getPathInfo().len)
			handler->computePathTranslated(bestLocation->getRoot(), serverConfig);
		return true;
	}

	return validateAndFinalizePhysicalPath(bestLocation, handler, newUri, error);
}


void ProcessRequestHandler::finalizeErrorState(RequestHandler* handler, 
											   int originalCode, 
											   short response) 
{
	int finalCode = (response != -1) ? response : originalCode;
	handler->setStatusError(finalCode);
}

bool ProcessRequestHandler::generateErrorPath(short originalCode,
											const clsServerConfig* serverConfig,
											RequestHandler* handler,
											HttpError &error)
{
	const std::map<short, stErrorPagedata> &ErrorPagedata = serverConfig->getErrorPages();
	std::map<short, stErrorPagedata>::const_iterator it = ErrorPagedata.find(originalCode);

	if (it == ErrorPagedata.end())
	{
		finalizeErrorState(handler, originalCode, -1);
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

	s_uri_entry uri;
	uri.sv_raw_path = it->second.uri.sv_raw_path;

	if (!internalRedirect(uri, serverConfig, handler, error))
	{
		originalCode = error.getCodeStatus();   
		finalizeErrorState(handler, originalCode, it->second.response);
		return false;
	}

	finalizeErrorState(handler, originalCode, it->second.response);
	return true;
}

bool ProcessRequestHandler::processRequest(const RequestLine& startLine,
										   const clsServerConfig* serverConfig,
										   RequestHandler* handler)
{
	HttpError error;
	s_uri_entry uri;

	HelperFunctions::RemoveDotSegmentsDirect(startLine.getRequestURI().getPath().Data, 
											startLine.getRequestURI().getPath().len);

	const clsLocation* bestLocation = findBestLocation(
		serverConfig->getLocationExact(),
		serverConfig->getLocationPrefix(),
		startLine.getRequestURI().getPath()
	);

	if (!bestLocation)
	{
		error.setStatus(500, "Internal Server Error");
		return (handler->setError(error), false);
	}

	if (!isMethodAllowed(startLine.getMethod(), bestLocation->getAllowMethods()))
	{
		error.setStatus(403, "Forbidden");
		handler->setError(error);
		return false;
	}

	uri.setSview(startLine.getRequestURI().getPath());
	handler->setMethod(startLine.getMethod());

	handler->setReturn((bestLocation->getReturn().code != -1) ? bestLocation->getReturn() : serverConfig->getReturn());
	
	if (handler->getReturn().code != -1)
		return true;

	if ((handler->getMethod() == HttpTables::M_POST))
	{
		if (!handler->ExtractCgiMetadata(uri, bestLocation->getCgiPass()))
		{
			handler->setUploadStore(&bestLocation->getUploadStore().getPath());
			if (bestLocation->getUploadStore().sv_raw_path.len)
			{
				HelperFunctions::join_views(handler->getPhysicalPath(), MAX_PATH_LEN, 
										  bestLocation->getUploadStore().sv_raw_path, uri.sv_raw_path);
				handler->setUploadLocation(&bestLocation->getUploadLocation());
				return true;
			}
			else
				return (error.setStatus(403, "Forbidden"), false);
		}
	}

	handler->setRequestUri(startLine.getRequestURI().getPath());

	if (!handlePath(bestLocation, serverConfig, handler, uri, error))
	{
		if (error.isError())
			return (handler->setError(error), false);
		return false;
	}

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
		return (error.setStatus(508, "Loop Detected"), false);

	newUri.AddRedirectCount();

	const clsLocation* newLocation = findBestLocation(
		serverConfig->getLocationExact(),
		serverConfig->getLocationPrefix(),
		newUri.getView()
	);

	if (!newLocation)
		return (error.setStatus(404, "Not Found"), false);

	handler->setReturn((newLocation->getReturn().code != -1) ? newLocation->getReturn() : serverConfig->getReturn());
	handler->setRequestUri(newUri.getView());

	if (handler->getReturn().code != -1)
		return true;

	if (!handlePath(newLocation, serverConfig, handler, newUri, error))
		return false;

	handler->setError(error);
	return true;
}

void ProcessRequestHandler::convertToAbsUri(s_uri_entry& entry, const s_view& host, const std::string& port)
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

stReturnData ProcessRequestHandler::buildReturnFromPathAndStatus(s_uri_entry& newUri, 
																 short codeStatus, 
																 s_view &Host,
																 const std::string &Port)
{
	stReturnData returnData;
	returnData.code = codeStatus;

	if (newUri.flags.is_dir || newUri.flags.is_abs_path)
	{
		convertToAbsUri(newUri, Host, Port);
	}

	returnData.value = newUri;
	return returnData;
}