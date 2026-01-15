#include "ServerSock.hpp"

ServerSock::ServerSock()
{
    this->_totalSocks = 0;
}

// free all data and closing all the fds of socket
ServerSock::~ServerSock()
{
    set<int>::iterator it = _Sockets.begin();
    set<int>::iterator end = _Sockets.end();

    while (it != end)
    {
        close(*it);
        it++;
    }

    std::cout << "-------------------------\n";
    std::cout << "--- ServerSock closed ---\n";
    std::cout << "-------------------------" << std::endl;
}

void ServerSock::removeSocket(int fd)
{
    this->_Sockets.erase(fd);
    close(fd);
}

// initilaizing the sturct with the port and ip of the passive socket
void ServerSock::_initializeSockaffr(unsigned short port, unsigned int ipV4)
{
    memset(&temp, 0, sizeof(temp));

    temp.sin_port = htons(port);
    temp.sin_addr.s_addr = htonl(ipV4);
    temp.sin_family = AF_INET;
}

// build single socket with socket and bind and listen system calls
int ServerSock::_buildSingleSocket(unsigned short port, unsigned int ipV4)
{
    int fdSock = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSock == -1)
        throw std::runtime_error("socket system call fail");

    _initializeSockaffr(port, ipV4);
    const sockaddr *addr = reinterpret_cast<const sockaddr *>(&temp);

    if (bind(fdSock, addr, sizeof(sockaddr_in)) == -1)
        throw std::runtime_error("bind system call fail");

    if (listen(fdSock, MAX_QUEUE) == -1)
        throw std::runtime_error("listen system call fail");

    return fdSock;
}

// builds sockets that exist in vector
void ServerSock::buildSockets(const vector<unsigned short> &ports, const vector<unsigned int> &ipV4)
{
    vector<unsigned short>::const_iterator it = ports.begin();
    vector<unsigned short>::const_iterator end = ports.end();

    vector<unsigned int>::const_iterator itIp = ipV4.begin();
    vector<unsigned int>::const_iterator endIp = ipV4.end();

    while (it != end && itIp != endIp)
    {
        try
        {
            int fd = _buildSingleSocket(*it, *itIp);
            _Sockets.insert(fd);
            _totalSocks++;
        }
        catch (const std::exception &e)
        {
            std::cout << COLOR_BOLD << COLOR_RED;
            std::cout << "Error : " << e.what() << ' ' << *itIp << ':' << *it << std::endl;
            std::cout << COLOR_RESET;
        }
        ++it, ++itIp;
    }

    if (_totalSocks == 0)
        throw std::runtime_error("all socket fail or empty input");

    // in debug
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "sockets are now in passive mode" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
}

// check is the socket exist in the server or not
bool ServerSock::isServerSocket(int fd)
{
    set<int>::iterator end = _Sockets.end();

    return (_Sockets.find(fd) != end);
}

// simple getter
const set<int> &ServerSock::getServerSockets()
{
    return this->_Sockets;
}