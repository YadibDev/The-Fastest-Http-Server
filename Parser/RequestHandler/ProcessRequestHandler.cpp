#include "ProcessRequestHandler.hpp"




ProcessRequestHandler::ProcessRequestHandler()
{
}


const clsLocation* findBestLocation(
    const std::vector<clsLocation>		&LocationExact,
	const std::vector<clsLocation>		&LocationPrefix,
    const std::string& uri)
{
    const clsLocation* best = NULL;

    if (LocationExact.size() > 0)
    {
        for (size_t i = 0; i < LocationExact.size(); i++)
        {
            if (uri == LocationExact[i].getLocationData().uri)
                return &LocationExact[i];
        }
    }

    size_t maxLen = 0;

    if (LocationPrefix.size() > 0)
    {
        const std::vector<clsLocation>& prefixVec = LocationPrefix;

        for (size_t i = 0; i < prefixVec.size(); i++)
        {
            const std::string& loc = prefixVec[i].getLocationData().uri;

            if (!uri.compare(0, loc.size(), loc))
            {
                if (loc.size() > maxLen)
                {
                    maxLen = loc.size();
                    best = &prefixVec[i];
                }
            }
        }
    }

    return best;
}

const std::string ProcessRequestHandler::getPathCgi(const std::string &uri, const std::map<std::string, std::string> &cgi_pass)
{
    const std::string empty = "";
    size_t extension = uri.find_last_of('.');
    if (extension == std::string::npos)
        return empty;
    std::string extensionStr = uri.substr(extension);
    std::map<std::string, std::string>::const_iterator it = cgi_pass.find(extensionStr);

    if (it != cgi_pass.end())
        return it->second;
    return empty;
}

std::string ProcessRequestHandler::selectMethod(eMethods method) {
    switch (method) {
        case GET:
            return "GET";
        case POST:
            return "POST";
        case DELETE:
            return "DELETE";
        default:
            return "UNKNOWN";
    }
}



bool    checkPath(const std::string &physicalPath)
{
    struct stat buffer;
    
    if (!stat(physicalPath.c_str(), &buffer))
    {
        if (!access(physicalPath.c_str(), R_OK))
            return (true);
    }
    return (false);
}

std::string ProcessRequestHandler::getIndex(const clsLocation* bestLocation, HttpError &error)
{
    const std::vector<std::string> &vindex = bestLocation->getIndex();
    
    std::string root = bestLocation->getRoot();

    for (size_t i = 0; i < vindex.size(); i++)
    {
        std::string physicalPath = root + "/" + vindex[i];
        if (checkPath(physicalPath))
            return (physicalPath);
    }
    error.setStatus(403, "Forbidden");
    return "";
}

std::string ProcessRequestHandler::creatPhysicalPath(const clsLocation* bestLocation, const std::string &uri, HttpError &error)
{
    std::string PhysicalPath = "";

    if (uri.size() >= 1 && uri[uri.size() - 1] == '/')
        return getIndex(bestLocation, error);
    std::string URI = uri.substr(bestLocation->getLocationData().uri.size());
    PhysicalPath = bestLocation->getRoot() + '/'+ URI;
    if (checkPath(PhysicalPath))
        return PhysicalPath;
    error.setStatus(403, "Forbidden");
    return "";
}

void ProcessRequestHandler::processRequest(const clsRequest& request, const clsServerConfig& serverConfigs, RequestHandler& handler)
{
    const clsLocation* bestLocation = findBestLocation(serverConfigs.getLocationExact(), serverConfigs.getLocationPrefix(), request._startLine.getPath());
    HttpError error;

    if (bestLocation)
    {
        handler.setPhysicalPath(creatPhysicalPath(bestLocation, request._startLine.getPath(), error));
        handler.setAutoIndex(bestLocation->getAutoIndex());
        handler.setAllowMethod(request._startLine.getMethod() == (bestLocation->getAllowMethods() & request._startLine.getMethod()));
        handler.setQuery(request._startLine.getQuery());
        handler.setVersion(request._startLine.getVersion());
        handler.setMethod(selectMethod(request._startLine.getMethod()));
        handler.setHeaders(request._headerParser.getHeaderValues());
        handler.setErrorPages(bestLocation->getErrorPages());
        handler.setPathCgi(getPathCgi(request._startLine.getPath(), bestLocation->getCgiPass()));
        handler.setReturn(bestLocation->getReturn());
        handler.setUploadStore(bestLocation->getUploadStore());
        handler.setError(error);
    }
    
}