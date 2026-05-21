
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
#include <vector>
#include "PartRespond/mainprocess/Webserv.hpp"
#include "Parser/ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "Parser/RequestHandler/ProcessRequestHandler.hpp"
#include "linker/clsFlow.hpp"
#include <csignal>
using namespace std;

void HandlerSignal(int sig)
{
    if (sig == SIGINT)
    {
        throw std::runtime_error("Exit\nProgram Exit with [ctr + c]");
    }
}

int main(int ac, const char *av[])
{
    const char *configFile = "configs/default.conf";
    long maxClients = 500;
    if (ac > 3)
    {
        std::cout << "./webserv [config file path] [optional max client default = 500]" << std::endl;
        return 1;
    }
    if (ac >= 2)
        configFile = av[1];
    if (ac == 3)
    {
        if (HelperFunctions::ConvertStrToNum(av[2], maxClients) == false || maxClients <= 0 || maxClients > 5000)
        {
            if (maxClients > 5000)
                std::cout << "you can't create more than 5000 clients"
            else
                std::cout << "fail to convert max client to number or max client <= 0" << std::endl;
            return 1;
        }
    }

    try
    {
        signal(SIGINT, HandlerSignal);
        clsFlow flow(configFile, maxClients);
        flow.EventLoop();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}