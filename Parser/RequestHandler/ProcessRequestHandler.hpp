#ifndef ProcessRequestHandler_HPP
#define ProcessRequestHandler_HPP

#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "../ParseRequest/Request/Request.hpp"
#include "RequestHandler.hpp"

class ProcessRequestHandler
{
	static const std::string	getPathCgi(const std::string &uri, const std::map<std::string, std::string> &cgi_pass);
	static std::string			selectMethod(eMethods method);
	static std::string			getIndex(const clsLocation* bestLocation, HttpError &error);
	static std::string			creatPhysicalPath(const clsLocation* bestLocation, const std::string &uri, HttpError &error);
	static std::string					handleDirectory(const clsLocation* bestLocation, HttpError &error);


	public:
		ProcessRequestHandler();
		static void processRequest(const clsRequest& request, const clsServerConfig& serverConfig, RequestHandler& handler);
};

#endif