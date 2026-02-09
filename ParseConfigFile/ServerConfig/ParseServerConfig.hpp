#ifndef SERVERCONFIGDATA_HPP
#define SERVERCONFIGDATA_HPP

#include <string>
#include <vector>
#include <map>
#include "../LocationConfig/LocationConfigData.hpp"

struct ListenInfo {
	std::string host;
	int         port;
};

class ServerConfig {
private:
	std::vector<ListenInfo>		_listens;
	std::vector<std::string>	_server_names;
	std::map<int, std::string>	_error_pages;
	size_t						_max_body_size;
	std::vector<LocationConfig>	_locations;

public:
	ServerConfig();
	~ServerConfig();

	void addLocation(const LocationConfig& loc) {
		_locations.push_back(loc);
	}

};

#endif