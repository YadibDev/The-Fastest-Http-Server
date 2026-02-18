#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "RequestHandler.hpp"
#include "../ParseRequest/Request.hpp"

class ProcessRequestHandler
{

    static bool MethodAllowed(const clsRequest& request, const clsServerConfig& serverConfig)
    {
        return (request._startLine.getMethod() | serverConfig.getAllowedMethods() )
    }
    public:
        ProcessRequestHandler();
        static void processRequest(const clsRequest& request, const std::vector<clsServerConfig>& serverConfigs, RequestHandler& handler);
};