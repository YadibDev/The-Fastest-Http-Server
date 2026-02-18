#include "ProcessRequestHandler.hpp"


const st_location* findBestLocation(
    const std::vector<std::vector<st_location> >& locations,
    const std::string& uri)
{
    const st_location* best = NULL;

    // المرحلة 1: Exact Match
    if (locations.size() > 0)
    {
        const std::vector<st_location>& exactVec = locations[0];

        for (size_t i = 0; i < exactVec.size(); i++)
        {
            if (uri == exactVec[i].value)
                return &exactVec[i];
        }
    }

    // المرحلة 2: Longest Prefix Match
    size_t maxLen = 0;

    if (locations.size() > 1)
    {
        const std::vector<st_location>& prefixVec = locations[1];

        for (size_t i = 0; i < prefixVec.size(); i++)
        {
            const std::string& loc = prefixVec[i].value;

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


ProcessRequestHandler::ProcessRequestHandler()
{
}

void ProcessRequestHandler::processRequest(const clsRequest& request, const std::vector<clsServerConfig>& serverConfigs, RequestHandler& handler)
{
    
}