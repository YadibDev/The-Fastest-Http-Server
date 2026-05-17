#ifndef PROCESS_REQUEST_HANDLER_HPP
#define PROCESS_REQUEST_HANDLER_HPP

#include <map>
#include <vector>
#include <string>
#include <sys/stat.h>

#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp" 
#include "../ParseRequest/Request/HttpTypes.hpp"
#include "../Header/HeaderFiles.hpp"
#include "../../Utils/HelperFunctions.hpp"
#include "PathResolver.hpp"
#include "RequestHandler.hpp"
#include "../ParseRequest/Request/RequestLine.hpp"

#define INTERNAL_LOOP 10

class RequestHandler; 

class ProcessRequestHandler
{
    static void                 finalizeErrorState(RequestHandler* handler,
                                                   int originalCode, 
                                                   short response);
    
    static bool                 isMethodAllowed(HttpTables::eMethod method, 
                                                uint8_t allowedMethods);
    
    static bool                 handleDirectory(const clsServerConfig* serverConfig,
                                                const clsLocation* bestLocation,
                                                RequestHandler* handler,
                                                s_uri_entry& newUri,
                                                HttpError &error);

    static bool                 handlePath(const clsLocation* bestLocation,
                                           const clsServerConfig* serverConfig,
                                           RequestHandler* handler,
                                           s_uri_entry& newUri,
                                           HttpError &error);
    
    static bool                 validateAndFinalizePhysicalPath(const clsLocation* bestLocation,
                                                               RequestHandler* handler,
                                                               s_uri_entry& newUri,
                                                               HttpError &error);

static bool handleCgi(const clsLocation* bestLocation, 
                      RequestHandler* handler, 
                      s_uri_entry& newUri, 
                      char *PhysicalPath, 
                      HttpError &error);
    
    static stReturnData         buildReturnFromPathAndStatus(s_uri_entry& newUri, 
                                                             short codeStatus, 
                                                             s_view &Host, 
                                                             const std::string &Port);
    
    static void                 convertToAbsUri(s_uri_entry& entry, 
                                                const s_view& host, 
                                                const std::string& port);

public:
    ProcessRequestHandler();

    static bool                 processRequest(const RequestLine& StartLine, 
                                               const clsServerConfig* serverConfig, 
                                               RequestHandler* handler);
    
    static bool                 generateErrorPath(short originalCode,
                                                  const clsServerConfig* serverConfig,
                                                  RequestHandler* handler, 
                                                  HttpError &error);
    
    static bool                 internalRedirect(s_uri_entry& newUri,
                                                 const clsServerConfig* serverConfig,
                                                 RequestHandler* handler,
                                                 HttpError& error);

    static const clsLocation* findBestLocation(const std::vector<clsLocation> &LocationExact,
                                                 const std::vector<clsLocation> &LocationPrefix,
                                                 const s_view &uri);
};

#endif