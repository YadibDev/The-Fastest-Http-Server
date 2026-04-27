
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
#include <csignal>
// include "Parser/RequestHandler/RequestHandler.hpp"
using namespace std;

// struct block
// {
//     // config file source
//     ServerSock Server;
//     Linker Manager;
// };

void function(int signal)
{
    (void)signal;
    exit(1);
}


void HandlerSignal(int sig)
{
    if (sig == SIGINT)
    {
        throw std::runtime_error("Exit\nProgram Exit with [ctr + c]");
    } 
}

#include "linker/clsFlow.hpp"

int main()
{
    try
    {
        signal(SIGINT, HandlerSignal);
        clsFlow flow;
        flow.EventLoop();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}