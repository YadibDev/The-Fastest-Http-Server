
#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include "server/clsEpollHandler.hpp"
#include "server/clsServerSock.hpp"
#include "linker/clsLinker.hpp"
#include <vector>
#include "PartRespond/mainprocess/Webserv.hpp"

using namespace std;

// struct block
// {
//     // config file source
//     ServerSock Server;
//     Linker Manager;
// };

int main()
{

    clsEpollHandler epoll;
    epoll_event ClientBuffer[100];

    vector<unsigned short> allPort;
    vector<unsigned int> allIps;
    allPort.push_back(8080);
    allIps.push_back(0);


    clsServerSock server;
    clsLinker ClientsLinker;


    server.buildSockets(allPort, allIps);
    epoll.addServerSockets(server);

    string respond = "";

    int n = 0;
    while ((n = epoll.tryPollNewClients(ClientBuffer, 100, -1)))
    {
        for (int i = 0; i < n; i++)
        {
 
            int fd = ClientBuffer[i].data.fd;
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            int newClient;
            if ((ClientBuffer[i].events | EPOLLIN) == EPOLLIN)
            {
                newClient = server.tryAcceptNewClient(fd, &addr);
                if (newClient == -1)
                {
                    if (newClient == -1)
                        std::cerr << "Accept Fail" << std::endl;
                    continue;
                }
                else if (newClient > 0)
                {
                    // flow of accept client jdid
                    cout << newClient << endl;
                    ClientsLinker.insertClient(newClient, addr);
                    epoll.addClient(newClient);
                }
                else
                {
                    newClient = fd;
                    string buffer;
                    buffer.resize(4096);
                    int size = recv(fd, &buffer[0], 4095, MSG_DONTWAIT);
                    if (size == 0)
                    {
                        close(fd);
                        continue;
                    }
                    buffer[size] = '\0';
                    std::cout << "read size" << size << std::endl;
                    std::cout << buffer << endl;
                    ClientsLinker.GetClientAt(newClient).SetState(REQUEST_MODE);
                    ClientsLinker.GetClientAt(newClient).ProcessRespond();
                    if (ClientsLinker.GetClientAt(newClient).GetState() != BEGIN)
                        epoll.changeAbility(newClient, EPOLLOUT);

                    // Response.~clsResponse();
                }
            }
            else if ((ClientBuffer[i].events | EPOLLOUT) == EPOLLOUT)
            {
                newClient = fd;

                ClientsLinker.GetClientAt(newClient).ProcessRespond();
                    if (ClientsLinker.GetClientAt(newClient).GetState() == BEGIN)
                        epoll.changeAbility(newClient, EPOLLIN);
            }
        }
    }
}