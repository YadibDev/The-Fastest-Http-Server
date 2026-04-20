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


bool	ProcessRequestHandler::handleDirectory(const clsLocation* bestLocation, char *destBuffer, HttpError &error) {
	const std::vector<std::string> &vindex = bestLocation->getIndex();
	const std::string &base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
	size_t baseLen = base.size();

	if (baseLen >= MAX_PATH_LEN - 1)
		return (error.setStatus(400, "Physical Path Is Large"), false);

	for (size_t i = 0; i < vindex.size(); i++) {
		memcpy(destBuffer, base.c_str(), baseLen);
		size_t currentPos = baseLen;

		if (currentPos > 0 && destBuffer[currentPos - 1] != '/') {
			if (currentPos < MAX_PATH_LEN - 1) destBuffer[currentPos++] = '/';
		}

		size_t idxLen = vindex[i].size();
		if (currentPos + idxLen < MAX_PATH_LEN) {
			memcpy(destBuffer + currentPos, vindex[i].c_str(), idxLen);
			destBuffer[currentPos + idxLen] = '\0';
			if (checkPath(destBuffer))
				return true;
		}
	}

	if (baseLen + 12 < MAX_PATH_LEN) {
		memcpy(destBuffer, base.c_str(), baseLen);
		size_t currentPos = baseLen;
		if (currentPos > 0 && destBuffer[currentPos - 1] != '/') destBuffer[currentPos++] = '/';
		memcpy(destBuffer + currentPos, "index.html", INDEX_PATH_LEN);
		if (checkPath(destBuffer))
			return true;
	}
	if (bestLocation->getAutoIndex()) {
		memcpy(destBuffer, base.c_str(), baseLen);
		destBuffer[baseLen] = '\0';
		return true;
	}
	return (error.setStatus(400, "Physical Path Is Large"), false);
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

	if (uri.len > 0 && uri.Data[uri.len - 1] == '/')
		return handleDirectory(bestLocation, destBuffer, error);

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

bool	checkExcute(s_view Path)
{
	char path[4096];
	struct stat st;
	if (Path.len < 4096)
	{
    	memcpy(path, Path.Data, Path.len);
    	path[Path.len] = '\0';

   		if (stat(path, &st) != 0)
   		    return false;

   		if (!S_ISREG(st.st_mode))
   		    return false;

   		return (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH));
	}

	return false;
}


bool ProcessRequestHandler::isMethodAllowed(HttpTables::eMethod method, uint8_t allowedMethods)
{
	uint8_t methodBit = (1 << method);

	return (methodBit == (allowedMethods & methodBit));
}

bool    ProcessRequestHandler::processRequest(const RequestLine& StartLine, const clsServerConfig* serverConfig, RequestHandler* handler)
{
	const clsLocation* bestLocation = findBestLocation(serverConfig->getLocationExact(), serverConfig->getLocationPrefix()
									, StartLine.getRequestURI().getPath());

	HttpError error;

	if (bestLocation)
	{
		if (!creatPhysicalPath(bestLocation, handler->getPhysicalPath(), StartLine.getRequestURI().getPath(), error))
			return (false);
		handler->setAutoIndex(bestLocation->getAutoIndex());
		if (!isMethodAllowed(StartLine.getMethod(), bestLocation->getAllowMethods()))
			return (error.setStatus(405, "Method Not Allowed"), handler->setError(error), false);

		handler->ExtractCgiMetadata(handler->getPhysicalPath(), bestLocation->getCgiPass());
		if (handler->getScriptName().len && !checkExcute(handler->getScriptName()))
			return (error.setStatus(403, "Forbidden"), handler->setError(error), false);

		if (handler->getPathCgi() && !handler->getPathCgi()->empty())
			handler->computePathTranslated(serverConfig->getRoot());
		if (!handler->getPathCgi() && !checkPath(handler->getPhysicalPath()))
				return (error.setStatus(404, "Not Found"), false);
	
		handler->setQuery(StartLine.getRequestURI().getQuery());
		handler->setVersion(StartLine.getVersion());
		handler->setMethod(StartLine.getMethod());
		handler->setErrorPages(bestLocation->getErrorPages());
		handler->setDefaultErrorPage(bestLocation->getDefaultErrorPage());
		handler->setReturn(bestLocation->getReturn());
		handler->setUploadStore(&bestLocation->getUploadStore());
		handler->setError(error);
	}
	return true;
}
