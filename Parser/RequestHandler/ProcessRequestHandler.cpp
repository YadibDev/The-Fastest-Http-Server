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
void	computePathTranslated(const std::string& rootPath, RequestHandler* handler)
{

	s_view PathInfo = handler->getPathInfo();
	size_t totalSize = rootPath.size() + PathInfo.len;

	bool needSlash = (!rootPath.empty() && rootPath[rootPath.size() - 1] != '/' && PathInfo.len > 0 && PathInfo.Data[0] != '/');

	if (needSlash)
		totalSize += 1;

	std::string fullPath;

	fullPath.reserve(totalSize);
	fullPath.append(rootPath);

	if (needSlash)
		fullPath += '/';

	fullPath.append(PathInfo.Data, PathInfo.len);

	handler->setPathTranslated(fullPath);
}

void	ProcessRequestHandler::getPathCgi(const s_view &uri, const std::map<std::string, std::string> &cgi_pass, RequestHandler* handler)
{
	s_view extView = HelperFunctions::find_last_of_view(uri, ".");
	handler->setPathInfo(HelperFunctions::find_last_of_view(uri, "/"));
	
	if (extView.Data == NULL || extView.len == 0 || extView.len > 255)
		return ;

	char buffer[256];
	size_t copyLen = extView.len - handler->getPathInfo().len;
	if (copyLen > 255)
		return ; // error exaction large
	memcpy(buffer, extView.Data, copyLen);
	buffer[copyLen] = '\0';

	std::string extensionStr(buffer);
	std::map<std::string, std::string>::const_iterator it = cgi_pass.find(extensionStr);

	if (it != cgi_pass.end())
		handler->setPathCgi(&it->second);
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


bool checkPath(const std::string &physicalPath) {
	struct stat buffer;

	return (stat(physicalPath.c_str(), &buffer) == 0 && (buffer.st_mode & S_IRUSR));
}

const char* ProcessRequestHandler::handleDirectory(const clsLocation* bestLocation, char *destBuffer, HttpError &error) {
	const std::vector<std::string> &vindex = bestLocation->getIndex();
	const std::string &base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
	size_t baseLen = base.size();

	if (baseLen >= 4095) return NULL;

	for (size_t i = 0; i < vindex.size(); i++) {
		memcpy(destBuffer, base.c_str(), baseLen);
		size_t currentPos = baseLen;

		if (currentPos > 0 && destBuffer[currentPos - 1] != '/') {
			if (currentPos < 4095) destBuffer[currentPos++] = '/';
		}

		size_t idxLen = vindex[i].size();
		if (currentPos + idxLen < 4096) {
			memcpy(destBuffer + currentPos, vindex[i].c_str(), idxLen);
			destBuffer[currentPos + idxLen] = '\0';
			if (checkPath(destBuffer))
				return destBuffer;
		}
	}

	if (baseLen + 12 < 4096) {
		memcpy(destBuffer, base.c_str(), baseLen);
		size_t currentPos = baseLen;
		if (currentPos > 0 && destBuffer[currentPos - 1] != '/') destBuffer[currentPos++] = '/';
		memcpy(destBuffer + currentPos, "index.html", 11);
		if (checkPath(destBuffer))
			return destBuffer;
	}

	if (bestLocation->getAutoIndex()) {
		memcpy(destBuffer, base.c_str(), baseLen);
		destBuffer[baseLen] = '\0';
		return destBuffer;
	}

	error.setStatus(403, "Forbidden");
	return NULL;
}

short    PercentEncoded(char *buffer, const s_view &uri)
{
	short Sv_index = 0;
	short Bufferindex = 0;

	char c = 0;

	while (Sv_index < uri.len)
	{
		c = uri.Data[Sv_index];
		if (uri.Data[Sv_index] == '%')
		{
			Sv_index++;
			c = HelperFunctions::hexToDecS_view(&uri.Data[Sv_index], 2);
			Sv_index++;
		}
		buffer[Bufferindex] = c;
		Sv_index++;
		Bufferindex++;
	}
	return Bufferindex;
}

const char* ProcessRequestHandler::creatPhysicalPath(const clsLocation* bestLocation, char *destBuffer, const s_view &uri, HttpError &error) {

	char    CleanUri[4095];

	if (uri.len > 0 && uri.Data[uri.len - 1] == '/')
		return handleDirectory(bestLocation, destBuffer, error);

	const std::string &base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
	size_t currentPos = base.size();

	if (currentPos >= 4095) return NULL;

	memcpy(destBuffer, base.c_str(), currentPos);
	size_t uriPartLen = PercentEncoded(CleanUri, uri);
	const char* uriPart = CleanUri;

	if (!bestLocation->getAlias().empty()) {
		size_t locSize = bestLocation->getLocationData().uri.size();
		if (uri.len >= locSize) {
			uriPart += locSize;
			uriPartLen -= locSize;
		}
	}

	if (uriPartLen > 0) {
		if (destBuffer[currentPos - 1] != '/' && uriPart[0] != '/') {
			if (currentPos < 4095)
				destBuffer[currentPos++] = '/';
		} 
		else if (destBuffer[currentPos - 1] == '/' && uriPart[0] == '/') {
			uriPart++;
			uriPartLen--;
		}

		if (currentPos + uriPartLen < 4096) {
			memcpy(destBuffer + currentPos, uriPart, uriPartLen);
			currentPos += uriPartLen;
		} else {
			error.setStatus(414, "URI Too Long");
			return NULL;
		}
	}

	destBuffer[currentPos] = '\0';

	if (checkPath(destBuffer))
		return destBuffer;

	error.setStatus(404, "Not Found");
	return NULL;
}

bool ProcessRequestHandler::isMethodAllowed(HttpTables::eMethod method, uint8_t allowedMethods)
{
	uint8_t methodBit = (1 << method);

	return (methodBit == (allowedMethods & methodBit));
}

void    ProcessRequestHandler::processRequest(const RequestLine& StartLine, const clsServerConfig* serverConfig, RequestHandler* handler)
{
	const clsLocation* bestLocation = findBestLocation(serverConfig->getLocationExact(), serverConfig->getLocationPrefix()
									, StartLine.getRequestURI().getPath());
	HttpError error;

	if (bestLocation)
	{
		creatPhysicalPath(bestLocation, handler->getPhysicalPath(), StartLine.getRequestURI().getPath(), error);
		handler->setAutoIndex(bestLocation->getAutoIndex());
		if (!isMethodAllowed(StartLine.getMethod(), bestLocation->getAllowMethods()))
		{
			error.setStatus(405, "Method Not Allowed");
			return ;
		}
		handler->setQuery(StartLine.getRequestURI().getQuery());
		handler->setVersion(StartLine.getVersion());
		handler->setMethod(StartLine.getMethod());
		handler->setErrorPages(bestLocation->getErrorPages());
		getPathCgi(StartLine.getRequestURI().getPath(), bestLocation->getCgiPass(), handler);
		if (handler->getPathCgi() && !handler->getPathCgi()->empty())
			computePathTranslated(serverConfig->getRoot(), handler);
		handler->setReturn(bestLocation->getReturn());
		handler->setUploadStore(&bestLocation->getUploadStore());
		handler->setError(error);
	}
	
}

