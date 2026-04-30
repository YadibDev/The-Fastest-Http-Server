#ifndef ProcessRequestHandler_HPP
#define ProcessRequestHandler_HPP

#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "../ParseRequest/Request/RequestParser.hpp"
#include "RequestHandler.hpp"
#include "../ParseRequest/Request/HttpTypes.hpp"
#include "../Header/HeaderFiles.hpp"
#include "../../Utils/HelperFunctions.hpp"
#include <sys/stat.h>

#define INDEX_PATH_LEN 11

class ProcessRequestHandler
{
	static void					finalizeErrorState(RequestHandler* handler, 
															int originalCode, 
											   				const stErrorPagedata& errorData);
	static void					getPathCgi(const s_view &uri, const std::map<std::string, std::string> &cgi_pass, RequestHandler* handler);
	// static std::string			selectMethod(Methods::eMethods method);
	static bool					isMethodAllowed(HttpTables::eMethod method, uint8_t allowedMethods);
	static std::string			getIndex(const clsLocation* bestLocation, HttpError &error);
	static bool					createPhysicalPath(const clsLocation* bestLocation, char *destBuffer, s_uri_entry& newUri, HttpError &error);
	static bool					handleDirectory(const clsServerConfig* serverConfig,
											const clsLocation* bestLocation,
											RequestHandler* handler,
											s_uri_entry& newUri,
											HttpError &error);
	static	const clsLocation* findBestLocation(
	const std::vector<clsLocation>	&LocationExact,
	const std::vector<clsLocation>	&LocationPrefix,
	s_view							uri);
	static bool 				handlePath(const clsLocation* bestLocation,
										const clsServerConfig* serverConfig,
										RequestHandler* handler,
										s_uri_entry& newUri,
										HttpError &error);
	static bool					handleCgi(const clsLocation* bestLocation, RequestHandler* handler, s_uri_entry& newUri, char *PhysicalPath);
	static stReturnData			buildReturnFromPathAndStatus(s_uri_entry& newUri, short codeStatus, s_view &Host, const std::string &Port);
	static void					convertToAbsUri(s_uri_entry& entry, const s_view& host, const std::string& port);



	public:
		ProcessRequestHandler();
		static bool					processRequest(const RequestLine& StartLine, const clsServerConfig* serverConfig, RequestHandler* handler);
		static bool 				generateErrorPath(short originalCode,
											const clsServerConfig* serverConfig,
											 RequestHandler* handler, 
											 HttpError &error);
		static bool					internalRedirect(s_uri_entry& newUri,
										const clsServerConfig* serverConfig,
										RequestHandler* handler,
										HttpError& error);
};

#endif