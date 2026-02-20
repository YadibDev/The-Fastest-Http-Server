#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "RequestHandler.hpp"
#include "../ParseRequest/Request.hpp"

class ProcessRequestHandler
{
    static bool MethodAllowed(const clsRequest& request, const clsServerConfig& serverConfig);
    public:
        ProcessRequestHandler();
        static void processRequest(const clsRequest& request, const clsServerConfig& serverConfig, RequestHandler& handler);
};