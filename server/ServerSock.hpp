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
#include "../Error.hpp"

#define COLOR_RED   "\033[31m"
#define COLOR_BOLD  "\033[1m"
#define COLOR_RESET "\033[0m"

#define MAX_QUEUE 100 // back log or total penging connection
using namespace std;

class ServerSock
{
private:
    struct sockaddr_in temp;
    set<int> _Sockets;
    size_t _totalSocks;
    HttpError _statusError;

    int _buildSingleSocket(unsigned short , unsigned int );
    void _initializeSockaffr(unsigned short , unsigned int );
    bool _isServerSocket(int);

public:
    ServerSock();
    ~ServerSock();

    enum class StatusError {
        ACCEPT_FAIL = 0,
        NOT_SOCKET_SERVER,
    };

    void buildSockets(const vector<unsigned short> &ports, const vector<unsigned int> &ip);
    void removeSocket(int);
    int tryAcceptNewClient(int sockServer, sockaddr_in * addr);
    const set<int> &getServerSockets();
};

#endif