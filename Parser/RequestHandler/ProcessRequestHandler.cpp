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

bool checkPath(const char *str)
{
	struct stat buffer;

	return (stat(str, &buffer) == 0 && (buffer.st_mode & S_IRUSR));
}

bool ProcessRequestHandler::handleDirectory(const clsServerConfig* serverConfig,
											const clsLocation* bestLocation,
											RequestHandler* handler,
											s_uri_entry& newUri,
											HttpError &error)
{
	short	flagType = CaseOfUri::U_FILE;
	const std::vector<s_uri_entry> &vindex = bestLocation->getIndex();

	for (size_t i = 0; i < vindex.size(); ++i)
	{
		flagType = vindex[i].flags;
		if ((flagType & CaseOfUri::U_RELATIVE) == CaseOfUri::U_RELATIVE)
		{
			if (!createPhysicalPath(bestLocation, handler->getPhysicalPath(), newUri, error))
				continue ;
			HelperFunctions::joinArr(handler->getPhysicalPath(), &vindex[i].getPath()[0], MAX_PATH_LEN);
			if (!checkPath(handler->getPhysicalPath()))
			{
				error.setStatus(403, "Forbidden");
				continue ;
			}
		}
		if (!internalRedirect(vindex[i], serverConfig, handler, error))
		{
			if ((flagType & CaseOfUri::U_RELATIVE) == CaseOfUri::U_RELATIVE)
				continue ;
			return false;
		}
		return true;
	}

	if (bestLocation->getAutoIndex())
	{
		handler->setAutoIndex(true);
		return true;
	}

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
		if (newUri.getView().len >= locSize)
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

bool	checkExecute(char *path)
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

bool	ProcessRequestHandler::handleCgi(const clsLocation* bestLocation, RequestHandler* handler, s_uri_entry& newUri, char *PhysicalPath)
{
	HttpError	error;
	handler->ExtractCgiMetadata(newUri, bestLocation->getCgiPass());

	if (handler->getScriptName().len)
	{
		if (!createPhysicalPath(bestLocation, PhysicalPath,
								handler->getScriptName(), error))
			return (handler->setError(error), false);

		if (!checkExecute(PhysicalPath))
			return (error.setStatus(403, "Forbidden"), handler->setError(error), false);
	}
	return true;
}

bool ProcessRequestHandler::handlePath(const clsLocation* bestLocation,
										const clsServerConfig* serverConfig,
										RequestHandler* handler,
										s_uri_entry& newUri,
										HttpError &error)
{

	if (newUri.getView().len && newUri.getView().Data[newUri.getView().len - 1] == '/')
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

	if (!createPhysicalPath(bestLocation, handler->getPhysicalPath(), newUri, error))
		return false;

	if (!checkPath(handler->getPhysicalPath()))
		return (error.setStatus(404, "Not Found"), false);

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
	const std::map<short, stErrorPagedata> &ErrorPagedata = serverConfig->getErrorPages();

	std::map<short, stErrorPagedata>::const_iterator it = ErrorPagedata.find(originalCode); // learn this 

	if (it == ErrorPagedata.end())
	{
		finalizeErrorState(handler, originalCode, stErrorPagedata());
		return false;
	}

	if ((it->second.uri.flags & CaseOfUri::U_ABS_URI) == CaseOfUri::U_ABS_URI)
	{
		stReturnData returnData;
		returnData.code = it->second.response;
		returnData.value = it->second.uri;
		handler->setReturnVal(returnData);
		return true;
	}

	const stErrorPagedata &foundData = it->second;
	s_uri_entry errorUri;

	errorUri.raw_path = foundData.uri;
	errorUri.initView();

	if (internalRedirect(errorUri, serverConfig, handler, error))
	{
		finalizeErrorState(handler, originalCode, foundData);
		return true;
	}

	finalizeErrorState(handler, originalCode, foundData);
	return false;
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
		return true;

	if (!isMethodAllowed(startLine.getMethod(), bestLocation->getAllowMethods()))
	{
		error.setStatus(405, "Method Not Allowed");
		handler->setError(error);
		return false;
	}

	uri.setSview(startLine.getRequestURI().getPath());
	if (!internalRedirect(uri, serverConfig, handler, error))
	{
		if (error.isError())
			return (handler->setError(error), false);
		return false;
	}

	handler->setQuery(startLine.getRequestURI().getQuery());
	handler->setVersion(startLine.getVersion());
	handler->setMethod(startLine.getMethod());
	return true;
}


bool ProcessRequestHandler::internalRedirect(
	s_uri_entry& newUri,
	const clsServerConfig* serverConfig,
	RequestHandler* handler,
	HttpError& error)
{
	handler->reset();

	newUri.redirect_count++;
	const clsLocation* newLocation = findBestLocation(
		serverConfig->getLocationExact(),
		serverConfig->getLocationPrefix(),
		newUri.getView()
	);
	if (!newLocation)
		return (error.setStatus(404, "Not Found"), false);

	handler->setReturn((!serverConfig->getReturn().value.empty()) ? serverConfig->getReturn() : newLocation->getReturn());

	if (!handler->getReturn().value.empty())
		return true;

	if (!handlePath(newLocation, serverConfig, handler, newUri, error))
		return false;

	handler->setUploadStore(&newLocation->getUploadStore());
	handler->setError(error);

	return true;
}