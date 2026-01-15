
#include "EpollHandler.hpp"

EpollHandler::EpollHandler(const set<int> &SocketsServer)
{
    this->_EpollFd = epoll_create(0);

    set<int>::iterator it = SocketsServer.begin();
    set<int>::iterator end = SocketsServer.end();
    while (it != end)
    {
        addClient(sock, EPOLLIN | EPOLLET, false)
        it++;
    }
}

EpollHandler::~EpollHandler ()
{

}

void EpollHandler::addClient(int fd, int ability, bool isClient = 1)
{

    this->tempEvent.data.fd = fd;
    tempEvent.data.ptr

    if (epoll_ctl (this->, EPOLL_CTL_ADD, fd, &reusableEvent) == -1)
        throw APPROPRIETE_ERROR

    if (isClient)
        this->_clientsDB.insert( {fd, CALL ClientData(fd)} ) // can be thrown an erro
        
}

END PROCEDURE
    
    
PROCEDURE removeClient(PARAMETER int fd):

    // i can remove the line below because the kernel remove it automatically IF i close the fd
    IF CALL epoll_ctl (..., EPOLL_CTL_DEL, fd, NULL) is -1
        throw APPROPRIETE_ERROR
    
    CALL this->_clientsDB.erase(fd)
    CALL close (fd)

END PROCEDURE

PROCEDURE changeAbility(PARAMETER int fd, PARAMETER int newAbility)

    SET reusableEvent.data.fd = fd
    SET reusableEvent.events = newAbility

    IF CALL epoll_ctl (..., EPOLL_CTL_MOD, fd, &reusableEvent) is -1
        throw APPROPRIETE_ERROR

END PROCEDURE


PROCEDURE checkTimeout():

    ...

END PROCEDURE

// ---------------------------------------------------------
// 3. FUNCTIONS (Methods with Return Value)
// ---------------------------------------------------------

FUNCTION tryPollNewClients(PARAMETER epoll_events ClientBuffer[], PARAMETER size_t size, int timeout) RETURN size_t:

    DECLARE size_t totalReadyFds;
    
    SET totalReadyFds = CALL epoll_wait(..., ClientBuffer, size, timeout)

    RETURN totalReadyFds

END FUNCTION