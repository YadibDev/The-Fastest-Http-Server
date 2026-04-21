#ifndef __ServerSock_HPP___
#define __ServerSock_HPP___

#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <vector>
#include <set>
#include <unistd.h>
#include <netinet/in.h>
#include "../Utils/HttpError.hpp"
#include "../Parser/ParseConfigFile/ServerConfig/ServerConfig.hpp"

#define COLOR_RED "\033[31m"
#define COLOR_BOLD "\033[1m"
#define COLOR_RESET "\033[0m"

#define MAX_QUEUE 400 // back log or total pending connection
using namespace std;

class clsServerSock
{
private:
    struct sockaddr_in temp;
    bool closeAtEnd;
    size_t _totalInterfaces;
    size_t _totalSocks;
    set<int> _Sockets;
    clsServerConfig *block;
    vector<unsigned int> _allIps;
    vector<unsigned short> _allPorts;
    int _buildSingleSocket(sockaddr_in &temp);
    // void _initializeSockaffr(unsigned short, unsigned int);
    bool _isServerSocket(int);

public:
    clsServerSock();
    ~clsServerSock();

    void enableCloseAtEnd();
    bool isServerIp(unsigned int ip, unsigned int port);
    void buildSockets(std::vector<sockaddr_in> listens);
    void removeSocket(int);
    void setBlock(clsServerConfig *block);
    clsServerConfig *getBlock();
    int tryAcceptNewClient(int sockServer, sockaddr_in *addr);
    const set<int> &getServerSockets();
};

#endif