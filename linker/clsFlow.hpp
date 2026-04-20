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
#include <stack>
#include "../server/clsEpollHandler.hpp"
#include "../server/clsServerSock.hpp"
#include "clsLinker.hpp"

#define EVENTS_MAX 200

// #include <vector>
// #include "PartRespond/mainprocess/Webserv.hpp"
// #include "Parser/ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
// #include "Parser/RequestHandler/ProcessRequestHandler.hpp"
// #include <csignal>

class clsFlow
{
public:
    epoll_event _clientsEvents[EVENTS_MAX];
    clsEpollHandler _epoll;
    std::vector<clsServerConfig> *_allBlocks;
    std::vector<clsServerSock> _allServers;
    std::map<short, short> clientIdByFd;
    clsClient *_clientsArr;
    std::stack<short> _clientsAvailable;
    size_t _totalServers;

    void _initializeStatics();
    void _createBlocksServers();
    void _createServers();
    void _initializeDataBase();
    short _getClient();
    void _pushClient(short clientId);
};