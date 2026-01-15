#ifndef __EPOLL_HANDLER_HPP___
#define __EPOLL_HANDLER_HPP___

#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <set>
#include <netinet/in.h>

using namespace std;

class EpollHandler
{
private:
    int _EpollFd;
    struct epoll_event tempEvent;    
public:
    EpollHandler(const set<int> &SocketsServer);
    ~EpollHandler();

    void addClient(int fd, int ability, bool isClient = true);

    void removeClient(int fd);
    void changeAbility(int fd, int newAbility);
    size_t tryPollNewClients(struct epoll_events ClientBuffer[], size_t size, int timeout);

};

#endif