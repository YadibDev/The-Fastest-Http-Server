#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <exception>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include "../server/clsEpollHandler.hpp"
#include "../server/clsServerSock.hpp"
#include "clsLinker.hpp"
// #include <vector>
// #include "PartRespond/mainprocess/Webserv.hpp"
// #include "Parser/ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
// #include "Parser/RequestHandler/ProcessRequestHandler.hpp"
// #include <csignal>

class clsFlow
{
    public:
    void _initializeStatics();
    void _createBlocksServers();
    void _createServers();
    size_t totalServers;
    std::vector<clsServerConfig> *_allBlocks;
    std::vector<clsServerSock> _allServers;
    clsEpollHandler _epoll;

   
};