#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "../../Utils/HttpError.hpp"
#include "../Header/HeaderFiles.hpp"
#include "../ParseRequest/Request/HttpTypes.hpp"
#include "../ParseRequest/Request/HeaderTable.hpp"
#include <map>
#include <vector>
#include <string>

#define MAX_PATH_LEN 4096

class RequestHandler {
private:

	uint8_t								_allowMethods;
	char								_physicalPath[MAX_PATH_LEN];
	bool								_autoindex;
	s_view								_query;
	s_view								_version;
	s_view								_PathInfo;
	s_view								_ScriptName;
	s_view								_ServerPort;
	std::string							_PathTranslated;
	stReturnData						_return;
	std::string							_body;
	std::string							_filePathBody;
	HttpTables::eMethod					_method;
	HeaderTable							_Header;
	const stErrorPagedata*				_defaultErrorPage;
	const std::string*					_pathCgi;
	const std::string*					_upload_store;
	HttpError							_error;
	short								_statusError;

public:
	RequestHandler(stPollRequest& request);
	~RequestHandler();

	void					reset();

	void					setAutoIndex(bool autoindex);
	void					setQuery(const s_view query);
	void					setVersion(const s_view version);
	void					setPathInfo(s_view pathInfo);
	void					setPathTranslated(std::string pathTranslated);
	void					setServerPort(const s_view serverPort);
	void					setMethod(HttpTables::eMethod method);
	void					setAllowedMethods(uint8_t allowed);
	void					setHeader(HeaderTable	_Header);
	void					setDefaultErrorPage(const stErrorPagedata* defaultErrorPage);
	void					setPathCgi(const std::string* pathCgi);
	void					setReturn(const stReturnData& returnData);
	void					setUploadStore(const std::string* uploadStore);
	void					setBody(const std::string& body);
	void					setFilePathBody(const std::string& filePathBody);
	void					setStatusError(short statusError);
	void					setError(const HttpError &error);

	bool					ExtractCgiMetadata(const s_view &uri, const std::map<std::string, std::string> &cgi_pass);
	bool					HandlerCgi(const s_view &uri, const std::map<std::string, std::string> &cgi_pass);
	void					computePathTranslated(const std::string& rootPath);
	// edited by achraf i add const 
	char*					getPhysicalPath() ;
	const char*				getPhysicalPath() const;
	bool					getAutoIndex() const;
	const s_view&			getQuery() const;
	const s_view&			getVersion() const;
	const s_view&			getScriptName() const;
	const s_view&			getPathInfo() const;
    const std::string&		getPathTranslated() const;
    const s_view&			getServerPort() const;
	HttpTables::eMethod		getMethod() const;
	const HeaderTable		&getHeader() const;
	const std::string*		getPathCgi() const;
	const stReturnData&		getReturn() const;
	const std::string*		getUploadStore() const;
	const std::string&		getBody() const;
	const std::string&		getFilePathBody() const;
	short					getStatusError();
	const HttpError&		getError() const;
};

#endif