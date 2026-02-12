#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "LocationConfigData.hpp"
#include "../../Utils/HttpError.hpp"
#include <iostream>

	enum e_method {
    	GET = 1,
    	POST = 2,
    	DELETE = 4
	};

class clsParseLocationConfig
{
	private:

		enum RequestState {
			READING_LINE,
			READING_HEADERS,
			READING_BODY,
			COMPLETED,
			ERROR
		};

		clsLocationConfig	LocationConfig;
		enum RequestState	State;

		HttpError parseRequestLine ();
	public:
		clsParseLocationConfig() : _state(READING_LINE);
		HttpError ParseRequest(std::string &rawData);
	
}
_index


#include <string>
#include <vector>
#include <map>

struct stReturnData
{    
	short       code;
	std::string value;

    stReturnData() : code(0), value("") {}	
};

class clsLocationConfig {
private:

	std::string							_path;
	std::string							_root;
	std::vector<std::string>			_methods;
	std::vector<std::string>			_index;
	bool								_autoindex;
	std::string							_return_url;
	std::string							_upload_store;
	std::map<std::string, std::string>	_cgi_info;

public:
	clsLocationConfig();
	~clsLocationConfig();

	void setPath(const std::string& path);
	void setRoot(const std::string& root);
	void setMethods(const std::vector<std::string>& methods);
	void setIndex(const std::string& index);
	void setAutoindex(bool autoindex);
	void setReturnUrl(const std::string& url);
	void setUploadStore(const std::string& store);
	void addCgi(const std::string& extension, const std::string& binPath);
	void addMethod(const std::string& method);

	const std::string& getPath() const;
	const std::string& getRoot() const;
	const std::vector<std::string>& getMethods() const;
	const std::string& getIndex() const;
	bool getAutoindex() const;
	const std::string& getReturnUrl() const;
	const std::string& getUploadStore() const;
	const std::map<std::string, std::string>& getCgiInfo() const;
};

#endif