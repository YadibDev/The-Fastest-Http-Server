#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "../Header/HeaderFiles.hpp"

class RequestHandler {
private:
	std::string _physicalPath;
	bool        _autoindex;
	bool		_allowMethod;
	std::string _query;
	std::string _version;
	std::string _method;
	std::map<std::string, std::vector<std::string> > _headers;
	std::map<short, stErrorPagedata> _error_pages;
	std::string _pathCgi;
	stReturnData _return;
	std::string _upload_store;
	std::string _body;
	std::string _filePathBody;
	HttpError	_error;

public:
	RequestHandler();
	~RequestHandler();


	bool	MethodAllowed() const;


	void setPhysicalPath(const std::string& path);
	void setAutoIndex(bool autoindex);
	void setAllowMethod(bool allowMethod);
	void setQuery(const std::string& query);
	void setVersion(const std::string& version);
	void setMethod(const std::string& method);
	void setHeaders(const std::map<std::string, std::vector<std::string> >& headers);
	void setErrorPages(const std::map<short, stErrorPagedata>& errorPages);
	void setPathCgi(const std::string& pathCgi);
	void setReturn(stReturnData returnData);
	void setUploadStore(const std::string& uploadStore);
	void setBody(const std::string& body);
	void setFilePathBody(const std::string& filePathBody);
	void setError(HttpError	&error);

	const std::string& getPhysicalPath() const;
	bool getAutoIndex() const;
	bool getAllowMethod() const;
	const std::string& getQuery() const;
	const std::string& getVersion() const;
	const std::string& getMethod() const;
	std::vector<std::string> getHeaderValues(const std::string& key) const;
	stErrorPagedata getErrorPage(short code) const;
	const std::string& getPathCgi() const;
	const stReturnData& getReturn() const;
	const std::string& getUploadStore() const;
	const std::string& getBody() const;
	const std::string& getFilePathBody() const;
	const stReturnData& getReturnData() const;
	const HttpError &getError() const;

};

#endif