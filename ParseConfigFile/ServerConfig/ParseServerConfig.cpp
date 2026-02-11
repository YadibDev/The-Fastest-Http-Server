#include "ParseServerConfig.hpp"

clsServerConfig::ServerConfig(clsParse<TokenType>	Parse) : _max_body_size(1048576), _Parse(Parse) {}

clsServerConfig::~ServerConfig() {}

void    clsServerConfig::addLocation(){
	_locations.push_back(loc);
}

HttpError	parseBlockServer()
{
	if (_Parse.peek().)
}