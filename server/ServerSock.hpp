#ifndef __ServerSock_HPP___
#define __ServerSock_HPP___

#include <iostream>
#include <string>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <unistd.h>
#include <netinet/in.h>

#define MAX_QUEUE 100 // back log or total penging connection
using namespace std;

class ServerSock
{
private:
    struct sockaddr_in temp;
    set<int> _Sockets;
    int _buildSingleSocket(unsigned short port, unsigned int ipV4);
    size_t _totalSocks;
    void _initializeSockaffr(unsigned short port, unsigned int ipV4);

public:
    ServerSock();
    ~ServerSock();

    void buildSockets(const vector<unsigned short> &ports, unsigned int ipV4);
    bool isServerSocket(int);
    const set<int> &getServerSockets();
};

#endif