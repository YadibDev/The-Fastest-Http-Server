#include "EpollHandler.hpp"

EpollHandler::EpollHandler()
{
    this->_EpollFd = epoll_create(1);  // epoll_create was use the flag size to pre-allocate the size you send to it , to use it after

    if (this->_EpollFd == -1)
    {
        clog << "epoll_create fail" << std::endl;
        return ;        // should think about this edge case and do a solution like in the total watching list 0 fds
    }
}

EpollHandler::~EpollHandler ()
{
    if (this->_EpollFd != -1)
        close(_EpollFd);
}

bool EpollHandler::addServerSockets(ServerSock &SocketsServer, int ability)
{
    size_t total = 0;

    set<int> socksFd = SocketsServer.getServerSockets();
    set<int>::iterator it = socksFd.begin();
    set<int>::iterator end = socksFd.end();

    while (it != end)
    {
        if (addClient(*it, ability)) // did i must add this ==> 'EPOLLET'
            total++;
        else
        {
            std::clog << *it << " ==> fail to add using epoll_ctl" << std::endl;
            SocketsServer.removeSocket(*it);
        }
        ++it;
    }

    if (total == 0)
    {
        std::clog << COLOR_BOLD << COLOR_RED<< "EPOLL_CTL_ADD FAIL TO ADD FDS to the watching list" << std::endl;
        return false;
    }
    return true;
}

//, bool isClient = 1
bool EpollHandler::addClient(int fd, int ability)
{
    memset(&tempEvent, 0, sizeof(tempEvent));
    tempEvent.data.fd = fd;
    tempEvent.events = ability;

    if (epoll_ctl (_EpollFd, EPOLL_CTL_ADD, fd, &tempEvent) == -1)
    {
        std::cout << "Fail to add new client in epoll" << std::endl;
        return false; // we can add here sterror and errno for debugin in future
    }
    return true;
}

void EpollHandler::changeAbility(int fd, int newAbility)
{
    memset(&tempEvent, 0, sizeof(tempEvent));
    tempEvent.data.fd = fd;
    tempEvent.events = newAbility;

    if (epoll_ctl (_EpollFd, EPOLL_CTL_MOD, fd, &tempEvent) == -1)
        clog << COLOR_BOLD << COLOR_RED << fd << " ==> fail to change mode EPOLL_CTL_MOD" << std::endl;
}

int EpollHandler::tryPollNewClients(struct epoll_event *ClientBuffer, size_t sizeBuffer, int timeout)
{
    int totalReadyFds;

    totalReadyFds = epoll_wait(_EpollFd, ClientBuffer, sizeBuffer, timeout);

    return totalReadyFds;
}