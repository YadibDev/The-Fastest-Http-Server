#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "../ParseRequest/Request/Request.hpp"
#include "RequestHandler.hpp"

class ProcessRequestHandler
{
    static const std::string getPathCgi(const std::string &uri, const std::map<std::string, std::string> &cgi_pass);
    static std::string selectMethod(eMethods method);

    public:
        ProcessRequestHandler();
        static void processRequest(const clsRequest& request, const clsServerConfig& serverConfig, RequestHandler& handler);
};