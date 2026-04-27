#include "clsServerSock.hpp"

enum StatusError
{
    DEFAULT = 0,
    ACCEPT_FAIL = 1,
    NOT_SOCKET_SERVER,
};

clsServerSock::clsServerSock() : _totalInterfaces(0), _totalSocks(0)
{
    closeAtEnd = false;
}

void clsServerSock::enableCloseAtEnd()
{
    closeAtEnd = true;
}

// free all data and closing all the fds of socket
void clsServerSock::freeAllSockets()
{
    set<int>::iterator it = _Sockets.begin();
    set<int>::iterator end = _Sockets.end();

    // std::cout << "------- DEBUG ---------\n" << std::endl;
    // std::cout << "------- " << _Sockets.size() << std::endl;
    while (it != end)
    {
        close(*it);
        it++;
    }
    _Sockets.clear();
}
clsServerSock::~clsServerSock()
{
    if (closeAtEnd == false)
        return;
    freeAllSockets();
}

void clsServerSock::removeSocket(int fd)
{
    this->_Sockets.erase(fd);
    this->_totalSocks--;
    close(fd);
}

// // initilaizing the sturct with the port and ip of the passive socket
// void clsServerSock::_initializeSockaffr(unsigned short port, unsigned int ipV4)
// {
//     memset(&temp, 0, sizeof(temp));

//     temp.sin_port = htons(port);
//     temp.sin_addr.s_addr = htonl(ipV4);
//     temp.sin_family = AF_INET;
// }

// build single socket with socket and bind and listen system calls
int clsServerSock::_buildSingleSocket(sockaddr_in &temp)
{
    unsigned short port = temp.sin_port;
    unsigned int ipV4 = temp.sin_addr.s_addr;

    int fdSock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fdSock == -1)
        throw std::runtime_error("socket system call fail");

    const sockaddr *addr = reinterpret_cast<const sockaddr *>(&temp);

    int enable = 1;
    // read about it 
    setsockopt(fdSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)); // i must read about SOL_SOCKET

    if (bind(fdSock, addr, sizeof(sockaddr_in)) == -1)
    {
        close(fdSock);
        throw std::runtime_error("bind system call fail");
    }

    if (listen(fdSock, MAX_QUEUE) == -1)
    {
        close(fdSock);
        throw std::runtime_error("listen system call fail");
    }

    this->_allIps.push_back(ipV4);
    this->_allPorts.push_back(port);
    _totalInterfaces++;
    return fdSock;
}
void clsServerSock::setBlock(clsServerConfig *block)
{
    this->block = block;
}

clsServerConfig *clsServerSock::getBlock()
{
    return block;
}
// builds sockets that exist in vector
void clsServerSock::buildSockets(std::vector<sockaddr_in> listens)
{

    int fd;
    for (size_t i = 0; i < listens.size(); i++)
    {
        try
        {
            fd = _buildSingleSocket(listens[i]);
            _Sockets.insert(fd);
            _totalSocks++;
        }
        catch (const std::exception &e)
        {
            std::cout << COLOR_BOLD << COLOR_RED;
            std::cout << "Error : " << e.what() << ' ' << listens[i].sin_addr.s_addr << ':' << listens[i].sin_port << std::endl;
            std::cout << COLOR_RESET;
        }
    }

    if (_totalSocks == 0)
        throw std::runtime_error("all socket fail or empty input");

    // in debug
    // std::cout << "-----------------------------------" << std::endl;
    // std::cout << "sockets are now in passive mode" << std::endl;
    // std::cout << "-----------------------------------" << std::endl;
}

// check is the socket exist in the server or not
bool clsServerSock::_isServerSocket(int fd)
{
    set<int>::iterator end = _Sockets.end();

    return (_Sockets.find(fd) != end);
}

bool clsServerSock::isServerIp(unsigned int ip, unsigned int port)
{
    for (size_t i = 0; i < _totalInterfaces; i++)
        if (ip == this->_allIps[i] && port == _allPorts[i])
            return true;

    return false;
}

int clsServerSock::tryAcceptNewClient(int sockServer, sockaddr_in *addr)
{
    if (_isServerSocket(sockServer) == false)
    {
        // std::cout << "Not server socket \n";
        return 0;
    }

    sockaddr *castIt = reinterpret_cast<sockaddr *>(addr);
    socklen_t temp = sizeof(sockaddr_in); // hold size of sockaddr_in temporary because i don't need it
    int fd = 0;

    if ((fd = accept(sockServer, castIt, &temp)) == -1)
    {
        return -1;
    }

    return fd;
}

// simple getter
const set<int> &clsServerSock::getServerSockets()
{
    return this->_Sockets;
}
