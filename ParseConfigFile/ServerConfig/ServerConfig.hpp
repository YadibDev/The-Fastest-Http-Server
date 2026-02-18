#ifndef SERVERCONFIGDATA_HPP
#define SERVERCONFIGDATA_HPP

#include <string>
#include <vector>
#include <map>
#include "../LocationConfig/LocationConfig.hpp"




class clsServerConfig {

private:
	std::vector<sockaddr_in>			_listens;
	// std::vector<std::string>		_server_names;
	std::map<short, stErrorPagedata>	_error_pages;
	size_t								_max_body_size;
	std::vector<clsLocation>			_LocationExact;
	std::vector<clsLocation>			_LocationPrefix;
	clsParse<TokenType>					_Parse;
	HttpError							_ERROR;
	
	bool				ParseListen(s_parse_context& ctx);
	// bool	ParseServerName(s_parse_context& ctx);
	bool				ParseErrorPage(s_parse_context& ctx);
	bool				ParseClientMaxBodySize(s_parse_context& ctx);
	bool				ParseLocation(s_parse_context& ctx);
	bool				ParseRoot(s_parse_context& ctx);

	enBlocksDirective	getServerDirectiveType(const std::string& key);
	bool				ParseServerDirective(s_parse_context& ctx);

public:

	clsServerConfig(clsParse<TokenType>	Parse);
	~clsServerConfig();

	bool	parseBlockServer();

	std::vector<ListenInfo>			getListens() const;
	// std::vector<std::string>		getServerNames() const;
	std::map<int, stErrorPagedata>	getErrorPages() const;
	size_t							getMaxBodySize() const;
	std::vector<clsLocation>		getLocationExact() const;
	std::vector<clsLocation>		getLocationPrefix() const;

	HttpError getError() const;
};

#endif