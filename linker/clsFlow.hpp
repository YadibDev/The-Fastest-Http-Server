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
#include <unistd.h>

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
    enum fdTypes {SERVER_SOCK, CLIENT_SOCK, PIPE};
    size_t _totalServers;
    clsClient *_clientsArr;
    std::vector<clsServerConfig> *_allBlocks;
    epoll_event _clientsEvents[EVENTS_MAX];
    clsEpollHandler _epoll;
    std::vector<clsServerSock> _allServers;
    std::map<short, short> _clientIdByFd;
    std::map<short, short> _IdByPipe;
    std::stack<short> _clientsAvailable;

    void _initializeStatics();
    void _createBlocksServers();
    void _createServers();
    void _initializeDataBase();
    void _registerServersSockets();
    short _getClient();
    void _freeClient(short clientFd);
    void _newClientProcess(int serverFd);
    void _clientProcess(int fd, uint32_t event);
    bool _eventsEroorHandle(epoll_event &client, fdTypes &TypeFd);
    void _flowProcess(int fd, fdTypes &TypeFd, int indexEvent);
    bool _insertClient(int newClient, sockaddr_in &addr, clsServerConfig *block);
    void _popPipe(short pipe);
    void _pushPipe(short pipe, short indexClient);
    void _pipeFlow(int fd);

public:
    clsFlow();
    ~clsFlow();
    void EventLoop();

};