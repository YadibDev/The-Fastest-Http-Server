#ifndef SERVERCONFIGDATA_HPP
#define SERVERCONFIGDATA_HPP

#include <string>
#include <vector>
#include <map>
#include "../LocationConfig/ParseLocationConfig.hpp"

struct ListenInfo {
	std::string host;
	int         port;
};

class clsServerConfig {

private:
	std::vector<ListenInfo>		_listens;
	std::vector<std::string>	_server_names;
	std::map<int, std::string>	_error_pages;
	size_t						_max_body_size;
	std::vector<LocationConfig>	_locations;
	clsParse<TokenType>			_Parse;

public:

	clsServerConfig(clsParse<TokenType>	Parse);
	~clsServerConfig();

	void addLocation(const LocationConfig& loc);

	HttpError	parseBlockServer();
};

#endif