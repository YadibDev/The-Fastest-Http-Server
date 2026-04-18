#ifndef __EPOLL_HANDLER_HPP___
#define __EPOLL_HANDLER_HPP___

#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <set>
#include <unistd.h>
#include <netinet/in.h>
#include "clsServerSock.hpp"

using namespace std;

class clsEpollHandler
{

private:
    int _EpollFd;
    struct epoll_event tempEvent;

public:
    clsEpollHandler();
    ~clsEpollHandler();

    bool addServerSockets(clsServerSock &SocketsServer, int ability = EPOLLIN);
    bool addClient(int fd, long long ability);

    void changeAbility(int fd, int newAbility);
    int tryPollNewClients(struct epoll_event *ClientBuffer, size_t size, int timeout);
};

#endif