#define CONFIGUEFILE_HPP
#ifndef CONFIGUEFILE_HPP

#include <iostream>
#include <string>
#include "../ParseServerConfig.hpp"

class ConfigueFile
{
    std::string RowData;
    std::vector<ServerConfig> servers;

    void    ParseConfigue()
    {
        while()
        {
            
        }
    }

    public:
        ConfigueFile (const std::string &fileName);
}

#endif