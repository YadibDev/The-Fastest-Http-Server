#ifndef __EPOLL_HANDLER_HPP___
#define __EPOLL_HANDLER_HPP___

#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <set>
#include <unistd.h>
#include <netinet/in.h>
#include "ServerSock.hpp"

using namespace std;

class EpollHandler
{

private:
    int _EpollFd;
    struct epoll_event tempEvent;    

public:
    EpollHandler();
    ~EpollHandler();

    bool addServerSockets(ServerSock &SocketsServer, int ability = EPOLLIN);
    bool addClient(int fd, int ability = EPOLLIN);

    void changeAbility(int fd, int newAbility);
    int tryPollNewClients(struct epoll_event *ClientBuffer, size_t size, int timeout);

};

#endif