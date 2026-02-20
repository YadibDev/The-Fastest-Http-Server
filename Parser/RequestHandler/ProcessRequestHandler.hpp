#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "../ParseRequest/Request/Request.hpp"
#include "RequestHandler.hpp"

class ProcessRequestHandler
{
    static bool MethodAllowed(const clsRequest& request, const clsServerConfig& serverConfig);
    public:
        ProcessRequestHandler();
        static void processRequest(const clsRequest& request, const clsServerConfig& serverConfig, RequestHandler& handler);
};