#include "ServerSock.hpp"

ServerSock::ServerSock()
{
    this->_totalSocks = 0;
}

ServerSock::~ServerSock()
{
    set<int>::iterator it = _Sockets.begin();
    set<int>::iterator end = _Sockets.end();

    while (it != end)
    {
        close(*it);
        it++;
    }

    std::cout << "----------------------------------------\n";
    std::cout << "--- ServerSock close all socket fds ---\n";
    std::cout << "----------------------------------------" << std::endl;
}

void ServerSock::_initializeSockaffr(unsigned short port, unsigned int ipV4)
{
    memset(&temp, 0, sizeof(temp));

    temp.sin_port = htons(port);
    temp.sin_addr.s_addr = htonl(ipV4);
    temp.sin_family = AF_INET;
}

int ServerSock::_buildSingleSocket(unsigned short port, int ipV4)
{
    int fdSock = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSock == -1)
        throw std::runtime_error("socket system call fail");

    _initializeSockaffr(port, ipV4);
    const sockaddr *addr = reinterpret_cast<const sockaddr *>(&temp);

    if (bind(fdSock, addr, sizeof(sockaddr_in)) == -1)
        throw std::runtime_error("socket system call fail");

    if (listen(fdSock, MAX_QUEUE) == -1)
        throw std::runtime_error("socket system call fail");
}

// map
void ServerSock::buildSockets(const vector<unsigned short> &ports, int ipV4)
{
    vector<unsigned short>::const_iterator it = ports.begin();
    vector<unsigned short>::const_iterator end = ports.end();

    int a = 5;

    while (it != end)
    {
        try
        {
            _buildSingleSocket(*it, ipV4);
            _totalSocks++;
        }
        catch (const std::exception &e)
        {
            std::cout << "Error :\n";
            std::cout << e.what() << std::endl;
        }
        ++it;
    }

    if (_totalSocks == 0)
        throw std::runtime_error("all socket fail or empty input");

    // in debug
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "all sockets are now in passive mode" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
}

bool ServerSock::isServerSocket(int fd)
{
}

const set<int> &ServerSock::getServerSockets()
{
}