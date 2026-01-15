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
    EpollHandler(ServerSock &SocketsServer);
    ~EpollHandler();

    bool addClient(int fd, int ability);

    void changeAbility(int fd, int newAbility);
    size_t tryPollNewClients(struct epoll_events ClientBuffer[], size_t size, int timeout);

};

#endif