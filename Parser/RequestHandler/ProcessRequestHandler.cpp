#include "ProcessRequestHandler.hpp"

ProcessRequestHandler::ProcessRequestHandler()
{
}


static bool	viewEqualsString(s_view view, const std::string &str)
{
	if (view.len != str.size())
		return false;
	return (std::memcmp(view.Data, str.data(), view.len) == 0);
}

static bool	viewStartsWithString(s_view view, const std::string &prefix)
{
	if (view.len < prefix.size())
		return false;
	return (std::memcmp(view.Data, prefix.data(), prefix.size()) == 0);
}

const clsLocation* ProcessRequestHandler::findBestLocation(
	const std::vector<clsLocation>	&LocationExact,
	const std::vector<clsLocation>	&LocationPrefix,
	s_view							uri)
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

std::string ProcessRequestHandler::selectMethod(Methods::eMethods method) {
	switch (method) {
		case Methods::GET:
			return "GET";
		case Methods::POST:
			return "POST";
		case Methods::DELETE:
			return "DELETE";
		default:
			return "UNKNOWN";
	}
}


bool checkPath(const char *str)
{
	struct stat buffer;

	return (stat(str, &buffer) == 0 && (buffer.st_mode & S_IRUSR));
}


// bool	ProcessRequestHandler::handleDirectory(const clsLocation* bestLocation, char *destBuffer, HttpError &error) {
// 	const std::vector<std::string> &vindex = bestLocation->getIndex();
// 	const std::string &base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
// 	size_t baseLen = base.size();

// 	if (baseLen >= MAX_PATH_LEN - 1)
// 		return (error.setStatus(400, "Physical Path Is Large"), false);

// 	for (size_t i = 0; i < vindex.size(); i++) {
// 		memcpy(destBuffer, base.c_str(), baseLen);
// 		size_t currentPos = baseLen;

// 		if (currentPos > 0 && destBuffer[currentPos - 1] != '/')
// 			if (currentPos < MAX_PATH_LEN - 1)
// 				destBuffer[currentPos++] = '/';

// 		size_t idxLen = vindex[i].size();
// 		if (currentPos + idxLen < MAX_PATH_LEN)
// 		{
// 			memcpy(destBuffer + currentPos, vindex[i].c_str(), idxLen);
// 			destBuffer[currentPos + idxLen] = '\0';
// 			if (checkPath(destBuffer))
// 				return true;
// 		}
// 	}

// 	if (baseLen + 12 < MAX_PATH_LEN)
// 	{
// 		memcpy(destBuffer, base.c_str(), baseLen);
// 		size_t currentPos = baseLen;
// 		if (currentPos > 0 && destBuffer[currentPos - 1] != '/') destBuffer[currentPos++] = '/';
// 		memcpy(destBuffer + currentPos, "index.html", INDEX_PATH_LEN);
// 		if (checkPath(destBuffer))
// 			return true;
// 	}
// 	if (bestLocation->getAutoIndex())
// 	{
// 		memcpy(destBuffer, base.c_str(), baseLen);
// 		destBuffer[baseLen] = '\0';
// 		return true;
// 	}
// 	return (error.setStatus(404, "Not Found"), false);
// }

bool ProcessRequestHandler::handleDirectory(const clsLocation* bestLocation,
											RequestHandler* handler,
											char *destBuffer,
											HttpError &error)
{
	size_t dirLen = std::strlen(destBuffer);

	s_view	index;
	const std::vector<std::string>& vindex = bestLocation->getIndex();

	for (size_t i = 0; i < vindex.size(); ++i)
	{
		size_t idxLen = vindex[i].size();
		if (dirLen + idxLen >= MAX_PATH_LEN)
			continue;
		index.Data = vindex[i].c_str();
		index.len = vindex[i].size();
		if (HandleCgi(bestLocation, handler, index, handler->getPhysicalPath()) && handler->getScriptName().len)
			return true;
		else if (handler->getScriptName().len)
			return false;
		std::memcpy(destBuffer + dirLen, vindex[i].c_str(), idxLen);
		destBuffer[dirLen + idxLen] = '\0';
		if (checkPath(destBuffer))
			return true; 
	}

	const char defaultIndex[] = "index.html";
	const size_t defaultLen = sizeof(defaultIndex) - 1;
	if (dirLen + defaultLen < MAX_PATH_LEN)
	{
		std::memcpy(destBuffer + dirLen, defaultIndex, defaultLen);
		destBuffer[dirLen + defaultLen] = '\0';
		if (checkPath(destBuffer))
			return true;
	}

	if (bestLocation->getAutoIndex()) // autoindex in this if write logic
		return true;

	return (error.setStatus(404, "Not Found"), false);
}

size_t PercentEncoded(char *buffer, size_t bufferSize, const s_view &uri)
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

bool ProcessRequestHandler::creatPhysicalPath(const clsLocation* bestLocation, char *destBuffer, const s_view &uri, HttpError &error) {

	char    CleanUri[MAX_PATH_LEN];

	const std::string &base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
	size_t currentPos = base.size();

	if (currentPos >= MAX_PATH_LEN - 1) return true;

	memcpy(destBuffer, base.c_str(), currentPos);
	size_t uriPartLen = PercentEncoded(CleanUri, MAX_PATH_LEN, uri);
	const char* uriPart = CleanUri;

	if (!bestLocation->getAlias().empty()) {
		size_t locSize = bestLocation->getLocationData().uri.size();
		if (uri.len >= locSize)
		{
			uriPart += locSize;
			uriPartLen -= locSize;
		}
	}

	if (uriPartLen > 0) {
		if (destBuffer[currentPos - 1] != '/' && uriPart[0] != '/') {
			if (currentPos < MAX_PATH_LEN - 1)
				destBuffer[currentPos++] = '/';
		} 
		else if (destBuffer[currentPos - 1] == '/' && uriPart[0] == '/') {
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

bool	checkExcute(char *path)
{
	struct stat st;

   	if (stat(path, &st) != 0)
   		return false;
   	if (!S_ISREG(st.st_mode))
   		return false;
   	return (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH));
}

bool ProcessRequestHandler::isMethodAllowed(HttpTables::eMethod method, uint8_t allowedMethods)
{
	uint8_t methodBit = (1 << method);

	return (methodBit == (allowedMethods & methodBit));
}

bool	ProcessRequestHandler::HandleCgi(const clsLocation* bestLocation, RequestHandler* handler, const s_view &requestUri, char *PhysicalPath)
{
	HttpError	error;
	handler->ExtractCgiMetadata(requestUri, bestLocation->getCgiPass());

	if (handler->getScriptName().len)
	{
		if (!creatPhysicalPath(bestLocation, PhysicalPath,
							   handler->getScriptName(), error))
			return (handler->setError(error), false);

		if (!checkExcute(PhysicalPath))
			return (error.setStatus(403, "Forbidden"), handler->setError(error), false);
	}
	return true;
}

bool ProcessRequestHandler::handlePath(const clsLocation* bestLocation,
						RequestHandler* handler,
						const s_view &requestUri, HttpError	error)
{

	if (!HandleCgi(bestLocation, handler, requestUri, handler->getPhysicalPath()))
			return false;
	else
	{
		if (!creatPhysicalPath(bestLocation, handler->getPhysicalPath(),
							   requestUri, error))
			return (false);

		if (requestUri.len && requestUri.Data[requestUri.len - 1] == '/')
			if (!handleDirectory(bestLocation, handler, handler->getPhysicalPath(), error)) // I didn't want to make the Physical path dependent on the class
				return false;
		if (!checkPath(handler->getPhysicalPath()))
			return (error.setStatus(404, "Not Found"), false);
	}

	if (handler->getPathInfo().len)
		handler->computePathTranslated(bestLocation->getRoot());

	return true;
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
	const map<short, stErrorPagedata> &ErrorPagedata = serverConfig->getErrorPages();

	std::map<short, stErrorPagedata>::const_iterator it = ErrorPagedata.find(originalCode); // learn this 

	handler->reset();

	if (it == ErrorPagedata.end())
	{
	    finalizeErrorState(handler, originalCode, stErrorPagedata());
	    return false;
	}

	const stErrorPagedata &foundData = it->second;
	s_view errorUri;
	
	errorUri.len = foundData.uri.length();
	errorUri.Data = foundData.uri.c_str();

	const clsLocation* bestLocation = findBestLocation(
		serverConfig->getLocationExact(),
		serverConfig->getLocationPrefix(),
		errorUri
	);
	if (bestLocation)
		handler->setReturn(bestLocation->getReturn());
	if (!bestLocation || !handlePath(bestLocation, handler, errorUri, error))
	{
		finalizeErrorState(handler, originalCode, foundData);
		return false;
	}

	// handler->setReturn(bestLocation->getReturn());
	finalizeErrorState(handler, originalCode, foundData);
	return true;
}

bool ProcessRequestHandler::processRequest(const RequestLine& startLine,
										   const clsServerConfig* serverConfig,
										   RequestHandler* handler)
{
	const clsLocation* bestLocation = findBestLocation(
		serverConfig->getLocationExact(),
		serverConfig->getLocationPrefix(),
		startLine.getRequestURI().getPath()
	);

	if (!bestLocation)
		return true;

	HttpError error;

	handler->setAutoIndex(bestLocation->getAutoIndex());

	if (!isMethodAllowed(startLine.getMethod(), bestLocation->getAllowMethods()))
	{
		error.setStatus(405, "Method Not Allowed");
		handler->setError(error);
		return false;
	}

	if (!handlePath(bestLocation, handler, startLine.getRequestURI().getPath(), error))
		return (handler->setError(error), false);

	handler->setQuery(startLine.getRequestURI().getQuery());
	handler->setVersion(startLine.getVersion());
	handler->setMethod(startLine.getMethod());
	handler->setDefaultErrorPage(bestLocation->getDefaultErrorPage());
	handler->setReturn(bestLocation->getReturn());
	handler->setUploadStore(&bestLocation->getUploadStore());

	handler->setError(error);
	return true;
}
