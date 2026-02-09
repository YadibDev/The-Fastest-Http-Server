#include "ParseServerConfig.hpp"

ServerConfig::ServerConfig() : _max_body_size(1048576) {}

ServerConfig::~ServerConfig() {}

void    ServerConfig::addLocation(){
        _locations.push_back(loc);
}