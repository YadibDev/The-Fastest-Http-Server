#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP


#include "../Header/HeaderFiles.hpp"
#include "../../Utils/HttpError.hpp"
#include "../ParseRequest/Request/HttpTypes.hpp"
#include "../ParseRequest/Request/HeaderTable.hpp"
#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "ProcessRequestHandler.hpp"
#include "PathResolver.hpp"
#include <map>
#include <vector>
#include <string>

#define MAX_PATH_LEN 4096
#define INTERNAL_LOOP 10

class RequestHandler {
private:

	uint8_t								_allowMethods;
	char								_physicalPath[MAX_PATH_LEN];
	bool								_autoindex;
	s_view								_requestUri;
	s_view								_query;
	s_view								_version;
	s_view								_PathInfo;
	s_view								_ScriptName;
	std::string							_PathTranslated;
	stReturnData						_return;
	std::string							_filePathBody;
	HttpTables::eMethod					_method;
	HeaderTable							_Header;
	bool								_defaultErrorPage;
	const std::string*					_pathCgi;
	const std::string*					_upload_store;
	HttpError							_error;
	short								_statusError;
	size_t								_sizeFile;

public:
	RequestHandler(stPollRequest& request);
	~RequestHandler();

	void					reset();

	void					setSizeFile(size_t sizeFile);
	void					setAutoIndex(bool autoindex);
	void					setRequestUri(const s_view &uri);
	void					setQuery(const s_view query);
	void					setVersion(const s_view version);
	void					setPathInfo(const s_view &pathInfo);
	void					setPathTranslated(std::string pathTranslated);
	void					setServerPort(const std::string &serverPort);
	void					setMethod(HttpTables::eMethod method);
	void					setAllowedMethods(uint8_t allowed);
	void					setHeader(HeaderTable	_Header);
	void					setDefaultErrorPage(bool defaultErrorPage);
	void					setPathCgi(const std::string* pathCgi);
	void					setReturn(const stReturnData& returnData);
	void					setReturnVal(stReturnData returnData);
	void					setUploadStore(const std::string* uploadStore);
	void					setFilePathBody(const std::string& filePathBody);
	void					setStatusError(short statusError);
	void					setError(const HttpError &error);

	bool					ExtractCgiMetadata(s_uri_entry& newUri, const std::map<std::string, std::string> &cgi_pass);
	bool					HandlerCgi(const s_view &uri, const std::map<std::string, std::string> &cgi_pass);
	void					computePathTranslated(const std::string& rootPath, const clsServerConfig* serverConfig);
	// edited by achraf i add const 

	size_t					getSizeFile() const;
	const s_view			&getRequestUri() const;
	char*					getPhysicalPath() ;
	const char*				getPhysicalPath() const;
	bool					getAutoIndex() const;
	const s_view&			getQuery() const;
	const s_view&			getVersion() const;
	const s_view&			getScriptName() const;
	const s_view&			getPathInfo() const;
    const std::string&		getPathTranslated() const;
    const std::string		&getServerPort() const;
	HttpTables::eMethod		getMethod() const;
	const HeaderTable		&getHeader() const;
	// edited by achraf
	HeaderTable    &getHeader();
	const std::string*		getPathCgi() const;
	bool					getDefaultErrorPage()const ;
	const stReturnData&		getReturn() const;
	const std::string*		getUploadStore() const;
	const std::string&		getFilePathBody() const;
	short					getStatusError();
	const HttpError&		getError() const;
};

#endif