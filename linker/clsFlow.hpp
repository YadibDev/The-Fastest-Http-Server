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

#define EVENTS_MAX 150
#define MAX_CLIENTS 500

// #include <vector>
// #include "PartRespond/mainprocess/Webserv.hpp"
// #include "Parser/ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
// #include "Parser/RequestHandler/ProcessRequestHandler.hpp"
// #include <csignal>

class clsFlow
{
private:
    size_t _totalServers;
    clsClient *_clientsArr;
    std::vector<clsServerConfig> *_allBlocks;
    epoll_event _clientsEvents[EVENTS_MAX];
    clsEpollHandler _epoll;
    std::vector<clsServerSock> _allServers;
    std::map<short, short> clientIdByFd;
    std::stack<short> _clientsAvailable;

    void _initializeStatics();
    void _createBlocksServers();
    void _createServers();
    void _initializeDataBase();
    void _registerServersSockets();
    short _getClient();
    void _pushClient(short clientId);
    void _clientFlow(short clientId);
    void _eventsEroorHandle()
public:
    clsFlow();
    void EventLoop(epoll_event &client);
};