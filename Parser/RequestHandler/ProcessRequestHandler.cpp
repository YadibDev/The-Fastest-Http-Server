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

            if (uri.compare(0, loc.size(), loc) == 0)
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

void ProcessRequestHandler::processRequest(const clsRequest& request, const clsServerConfig& serverConfigs, RequestHandler& handler)
{
    const clsLocation* bestLocation = findBestLocation(serverConfigs.getLocationExact(), serverConfigs.getLocationPrefix(), request._startLine.getPath());

    if (bestLocation)
    {
        handler.setPhysicalPath(bestLocation->getRoot() + request._startLine.getPath());
        handler.setAutoIndex(bestLocation->getAutoIndex());
        handler.setAllowMethod((bestLocation->getAllowMethods() & (1 << request._startLine.getMethod())) != 0);
        handler.setQuery(request._startLine.getQuery());
        handler.setMethod(static_cast<eMethods>(request._startLine.getMethod()));
        handler.setHeaders(request._headerParser.getHeaderValues());
        handler.setErrorPages(bestLocation->getErrorPages());
        handler.setPathCgi(bestLocation->getCgiPass().count(request._startLine.getPath()) > 0 ? bestLocation->getCgiPass().at(request._startLine.getPath()) : "");
        handler.setReturn(bestLocation->getReturn());
        handler.setUploadStore(bestLocation->getUploadStore());
    }
    
}