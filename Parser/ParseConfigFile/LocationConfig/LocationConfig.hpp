#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP


#include "../ServerConfig/ConfigDirectiveParser.hpp"


class clsLocation
{
private:

	
	s_parse_context						&ctx;
	stlocation							_locationData;
	std::string							_root;
	std::string							_alias;
	std::vector<std::string>			_index;
	bool								_autoindex;
	short								_allow_methods;
	unsigned long long					_client_max_body_size;
	stReturnData						_return;
	std::string							_upload_store;
	std::map<std::string, std::string>	_cgi_pass;
	std::map<short, stErrorPagedata>	_error_pages;


	bool	ParseRoot();

	bool	ParseAlias();

	bool	ParseIndex();

	bool	ParseAutoIndex();

	bool	ParseMethods();

	bool	ParseClientMaxBodySize();

	bool	ParseReturn();

	bool	ParseUploadStore();

	bool	ParseCgiPass();

	bool	ParseErrorPage();
	
	bool	ParseLocationDirective();
	
	enBlocksDirective	getLocationDirectiveType(const std::string& key);

public:

	clsLocation();
	clsLocation(s_parse_context& ctxs, const std::string &sRoot
				, const std::vector<std::string>& sIndex,  unsigned long long sClient_max_body_size, bool sAutoIndex);

	bool								parseLocation();
	
	std::string							getRoot() const;
	const std::string							&getAlias() const;
	const std::vector<std::string>			&getIndex() const;
	bool								getAutoIndex() const;
	short								getAllowMethods() const;
	unsigned long long					getClientMaxBodySize() const;
	stReturnData						getReturn() const;
	std::string							getUploadStore() const;
	const std::map<std::string, std::string>	&getCgiPass() const;
	const std::map<short, stErrorPagedata>	&getErrorPages() const;

	stlocation							getLocationData() const;
	HttpError							getError() const;

};

#endif
