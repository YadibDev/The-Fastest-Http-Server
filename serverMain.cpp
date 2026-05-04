
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
    if (ac > 2)
    {
        std::cout << "./webserv [you can add configFile as an argument]" << std::endl;
        return 1;
    }
    else if (ac == 2)
        configFile = av[1];

    try
    {
        signal(SIGINT, HandlerSignal);
        clsFlow flow(configFile);
        flow.EventLoop();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}