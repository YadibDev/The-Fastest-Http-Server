#ifndef ProcessRequestHandler_HPP
#define ProcessRequestHandler_HPP

#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "../ParseRequest/Request/RequestParser.hpp"
#include "RequestHandler.hpp"
#include "../ParseRequest/Request/HttpTypes.hpp"
#include "../../Utils/HelperFunctions.hpp"
#include <sys/stat.h>

#define INDEX_PATH_LEN 11

class ProcessRequestHandler
{
	static void					finalizeErrorState(RequestHandler* handler, 
															int originalCode, 
											   				const stErrorPagedata& errorData);
	static void					getPathCgi(const s_view &uri, const std::map<std::string, std::string> &cgi_pass, RequestHandler* handler);
	static std::string			selectMethod(Methods::eMethods method);
	static bool					isMethodAllowed(HttpTables::eMethod method, uint8_t allowedMethods);
	static std::string			getIndex(const clsLocation* bestLocation, HttpError &error);
	static bool					creatPhysicalPath(const clsLocation* bestLocation, char *destBuffer, const s_view &uri, HttpError &error);
	static bool					handleDirectory(const clsLocation* bestLocation, RequestHandler* handler, char *destBuffer, HttpError &error);
	static	const clsLocation* findBestLocation(
	const std::vector<clsLocation>	&LocationExact,
	const std::vector<clsLocation>	&LocationPrefix,
	s_view							uri);
	static bool					handlePath(const clsLocation* bestLocation,
									RequestHandler* handler,
									const s_view &requestUri,
									HttpError	error);
	static bool					HandleCgi(const clsLocation* bestLocation, RequestHandler* handler,
												const s_view &requestUri, char *PhysicalPath);


	public:
		ProcessRequestHandler();
		static bool					processRequest(const RequestLine& StartLine, const clsServerConfig* serverConfig, RequestHandler* handler);
		static bool 				generateErrorPath(short originalCode,
											const clsServerConfig* serverConfig,
											 RequestHandler* handler, 
											 HttpError &error);
};

#endif