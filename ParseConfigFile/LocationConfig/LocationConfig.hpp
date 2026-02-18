#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP


#include "../ServerConfig/ConfigDirectiveParser.hpp"


class clsLocation
{
private:

	
	s_parse_context						&ctx;
	stlocation							_locationData;
	std::string							_root;
	std::vector<std::string>			_index;
	bool								_autoindex;
	short								_allow_methods;
	unsigned long long					_client_max_body_size;
	stReturnData						_return;
	std::string							_upload_path;
	std::map<std::string, std::string>	_cgi_pass;
	std::map<short, stErrorPagedata>	_error_pages;


	bool	ParseRoot();

	bool	ParseIndex();

	bool	ParseAutoIndex();

	bool	ParseMethods();

	bool	ParseClientMaxBodySize();

	bool	ParseReturn();

	bool	ParseUploadPath();

	bool	ParseCgiPass();

	bool	ParseErrorPage();
	
	bool	ParseLocationDirective();
	
	enBlocksDirective	getLocationDirectiveType(const std::string& key);

public:

	clsLocation(s_parse_context& ctxs);

	bool								parseLocation();
	
	std::string							getRoot() const;
	std::vector<std::string>			getIndex() const;
	bool								getAutoIndex() const;
	short								getAllowMethods() const;
	unsigned long long					getClientMaxBodySize() const;
	stReturnData						getReturn() const;
	std::string							getUploadPath() const;
	std::map<std::string, std::string>	getCgiPass() const;
	std::map<short, stErrorPagedata>	getErrorPages() const;

	stlocation							getLocationData() const;
	HttpError							getError() const;
};

#endif
