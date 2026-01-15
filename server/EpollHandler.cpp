
#include "EpollHandler.hpp"

EpollHandler::EpollHandler(ServerSock &SocketsServer)
{
    size_t total = 0;

    // epoll_create was use the flag size to pre-allocate the size you send to it , to use it after
    this->_EpollFd = epoll_create(0);
    if (this->_EpollFd == -1)
        return ;        // should think about this edge case and do a solution like in the total watching list size == 0

    set<int> socksFd = SocketsServer.getServerSockets();
    set<int>::iterator it = socksFd.begin();
    set<int>::iterator end = socksFd.end();

    while (it != end)
    {
        if (addClient(*it, EPOLLIN)) // did i must add this ==> 'EPOLLET'
            total++;
        else
        {
            std:clog << *it << " ==> fail to add using epoll_ctl" << std::endl;
            SocketsServer.removeSocket(*it);
        }
        it++;
    }

    // if (total == 0)
    // i should handle the 0 epoll fd are watched by throwing an error or without it depend on the best
    // solution
}

EpollHandler::~EpollHandler ()
{
    if (this->_EpollFd != -1)
        close(_EpollFd);
}

// ability what the mode that epoll will be using for this fd

//, bool isClient = 1
bool EpollHandler::addClient(int fd, int ability)
{
    memset(&tempEvent, 0, sizeof(tempEvent));
    tempEvent.data.fd = fd;
    tempEvent.events = ability;

    if (epoll_ctl (_EpollFd, EPOLL_CTL_ADD, fd, &tempEvent) == -1)
        return false; // we can add here sterror and errno for debugin in future

    return true;
    // if (isClient)
    //     this->_clientsDB.insert( {fd, CALL ClientData(fd)} ) // can be thrown an erro will be used in monitor maybe

}

void EpollHandler::changeAbility(int fd, int newAbility)
{
    memset(&tempEvent, 0, sizeof(tempEvent));
    tempEvent.data.fd = fd;
    tempEvent.events = newAbility;

    if (epoll_ctl (_EpollFd, EPOLL_CTL_MOD, fd, &tempEvent) == -1)
        std:clog << COLOR_BOLD << COLOR_RED << fd << " ==> fail to change mode EPOLL_CTL_MOD" << std::endl;
}


// PROCEDURE checkTimeout():

//     ...

// END PROCEDURE

// // ---------------------------------------------------------
// // 3. FUNCTIONS (Methods with Return Value)
// // ---------------------------------------------------------

// FUNCTION tryPollNewClients(PARAMETER epoll_events ClientBuffer[], PARAMETER size_t size, int timeout) RETURN size_t:

//     DECLARE size_t totalReadyFds;
    
//     SET totalReadyFds = CALL epoll_wait(..., ClientBuffer, size, timeout)

//     RETURN totalReadyFds

// END FUNCTION