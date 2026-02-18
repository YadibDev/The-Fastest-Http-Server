#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include "server/EpollHandler.hpp"
#include "server/ServerSock.hpp"
#include <vector>
#include "PartRespond/mainprocess/Webserv.hpp"

using namespace std;

int main()
{

    ServerSock server;
    EpollHandler epoll;

    // clsResponse Response;


    string respond;

    vector<unsigned short> allPort;
    vector<unsigned int> allIps;

    allPort.push_back(8081);
    allIps.push_back(0);

    server.buildSockets(allPort, allIps);

    epoll.addServerSockets(server);

    epoll_event ClientBuffer[100];
    int n = 0;
    while ((n = epoll.tryPollNewClients(ClientBuffer, 100, -1)))
    {
        for (int i = 0; i < n; i++)
        {
            size_t ofset = 0;
            int fd = ClientBuffer[i].data.fd;
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            if ((ClientBuffer[i].events | EPOLLIN) == EPOLLIN)
            {
                int newClient = server.tryAcceptNewClient(fd, &addr);
                if (newClient == -1)
                {
                    if (newClient == -1)
                        std::cerr << "Accept Fail" << std::endl;
                    continue;
                }
                else if (newClient > 0)
                {
                    // flow of accept client jdid
                    epoll.addClient(newClient);
                }
                else
                {
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

                    clsResponse Response;

                    Response.SetFileFromDisk("index.html");
                    Response.SetMod(GET);
                    Response.SetStatus(200);
                    Response.SetType("text/html");
                    respond = "";

                    respond = Response.MakeResponse();
                    respond += Response.GetBody();
                    ofset = send(fd, respond.c_str(), respond.size(), MSG_DONTWAIT);
                    if (ofset < respond.size())
                        epoll.changeAbility(fd, EPOLLOUT);
                    close(fd);
                    // Response.~clsResponse();
                }
            }
            // else if ((ClientBuffer[i].events | EPOLLOUT) == EPOLLOUT)
            // {
            //     int ofset = 0;
            //     ofset = send(ClientBuffer[i].data.fd, &respond[ofset], respond.size() - ofset, MSG_DONTWAIT);
            //     if (ofset >= respond.size())
            //     {
            //         epoll.changeAbility(ClientBuffer[i].data.fd, EPOLLIN);
            //         ofset = 0;
            //     }
            // }
        }
    }
}