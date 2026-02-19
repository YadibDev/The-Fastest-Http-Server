#ifndef SERVERCONFIGDATA_HPP
#define SERVERCONFIGDATA_HPP

#include <string>
#include <vector>
#include <map>
#include "../LocationConfig/LocationConfig.hpp"


// struct ListenInfo {
// 	std::string host;
// 	int         port;
// };

class clsServerConfig {

private:
	std::vector<sockaddr_in>		_listens;
	// std::vector<std::string>		_server_names;
	std::string						_root;
	std::map<short, stErrorPagedata>	_error_pages;
	size_t							_max_body_size;
	std::vector<clsLocation>		_LocationExact;
	std::vector<clsLocation>		_LocationPrefix;
	s_parse_context					&ctx;
	
	bool				ParseListen();
	// bool	ParseServerName();
	bool				ParseErrorPage();
	bool				ParseClientMaxBodySize();
	bool				ParseLocation();
	bool				ParseRoot();

	enBlocksDirective	getServerDirectiveType(const std::string& key);
	bool				ParseServerDirective();

public:

	clsServerConfig(s_parse_context	&ctx);
	~clsServerConfig();

	bool	parseBlockServer();

	std::vector<sockaddr_in>		getListens() const; // address
	// std::vector<std::string>		getServerNames() const;
	std::map<short, stErrorPagedata>	getErrorPages() const;
	size_t							getMaxBodySize() const;
	std::vector<clsLocation>		getLocationExact() const;
	std::vector<clsLocation>		getLocationPrefix() const;
	std::string						getRoot() const;

	HttpError getError() const;
};

#endif