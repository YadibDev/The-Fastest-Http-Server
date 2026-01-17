

#include "ServerSock.hpp"
#include "EpollHandler.hpp"


int main()
{
    srand(time(NULL));
    ServerSock Server;

    vector<unsigned short> randomPorts;
    vector<unsigned int> randomIp;

    try
    {
        randomPorts.push_back(8080);
        randomIp.push_back(0);
        Server.buildSockets(randomPorts, randomIp);

        EpollHandler eventHandler(Server);

        epoll_event buff[10];
        
        
        eventHandler.tryPollNewClients(buff, 10, -1);

        sockaddr_in client;
        sockaddr_in server;
        socklen_t a;
        int fdClient = accept(*Server.getServerSockets().begin(), (sockaddr*) &client, &a);

        getsockname(fdClient, (sockaddr*) &server, &a);

        cout << "port client :\n"; 
        cout << client.sin_port << std::endl;
        short n;
        cout << "server client :\n"; 
        cout << ntohs(server.sin_port) << std::endl;
        

    }
    catch (std::exception &e)
    {
        std::cout << COLOR_BOLD << COLOR_RED;
        std::cout << "Error : " << e.what() << std::endl;
        std::cout << COLOR_RESET;
    }

}