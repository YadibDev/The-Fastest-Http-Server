
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
#include "Parser/ParseRequest/Request/Request.hpp"
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
    exit(1);
}

int main()
{
    signal(SIGINT, function);
    std::string Data;
    int fd = open("configs/default.conf", O_RDONLY);
    if (fd == -1)
    {
        std::cerr << "Error: Could not open config file." << std::endl;
        return 1;
    }

    ssize_t Size = 10000;
    HelperFunctions::ReadData(fd, Data, Size);
    close(fd);

    LexerConfig<TokenType> cfg(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);
    cfg.addCommentRule("#", "\n");
    cfg.addSeparatorToken('{', TOKEN_LBRACE);
    cfg.addSeparatorToken('}', TOKEN_RBRACE);
    cfg.addSeparatorToken(';', TOKEN_SEMICOLON);
    cfg.addSeparatorToken('\n', TOKEN_JOUJNO9ATE);
    cfg.addWithSpace(" \t");

    GenericLexer<TokenType> Lexer(Data, cfg);
    std::vector<Token<TokenType> > Tokens = Lexer.tokenize();
    clsParse<TokenType> Parse(Tokens, TOKEN_EOF);

    clsParseConfigueFile configFile(Parse);
    configFile.ParseConfigue();
    if (configFile.getServers().empty())
    {
        std::cout << "SEGFAULT HNA 58 main server\n";
        return 1;
    }
    clsServerConfig Block = configFile.getServers()[0];

    clsEpollHandler epoll;
    epoll_event ClientBuffer[100];

    clsServerSock server;
    clsLinker ClientsLinker;

    server.buildSockets(Block.getListens());
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
            if ((ClientBuffer[i].events & EPOLLRDHUP) == EPOLLRDHUP)
            {
                ClientsLinker.removeClient(fd);
                std::cout << "Fd" << fd << std::endl;
                continue;
            }
            else if ((ClientBuffer[i].events & EPOLLIN) == EPOLLIN)
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
                    epoll.addClient(newClient, EPOLLIN);
                }
                else
                {
                    newClient = fd;

                    clsClient &client = ClientsLinker.GetClientAt(newClient);

                    client.ProcessRequest();
                    if (client.GetState() == CONNECTION_CLOSED)
                    {
                        ClientsLinker.removeClient(newClient);
                        continue;
                    }
                    if (client.GetState() == START_RESPOND)
                    {
                        std::cout << "***********Done \n\n"
                                  << std::endl;
                        client.ProcessRespond(Block);
                        if (ClientsLinker.GetClientAt(newClient).GetState() != BEGIN)
                            epoll.changeAbility(newClient, EPOLLOUT);
                    }
                }
            }
            else if ((ClientBuffer[i].events & EPOLLOUT) == EPOLLOUT)
            {
                newClient = fd;
                std::cout << "RESPOND 2 start" << endl;
                RequestHandler reqHandler;

                try
                {
                    ClientsLinker.GetClientAt(newClient).ProcessRespond(Block);
                }
                catch (std::exception &e)
                {
                    cout << e.what() << endl;
                }
                if (ClientsLinker.GetClientAt(newClient).GetState() == BEGIN)
                    epoll.changeAbility(newClient, EPOLLIN);
                else if (ClientsLinker.GetClientAt(newClient).GetState() == CONNECTION_CLOSED)
                {
                    ClientsLinker.removeClient(newClient);
                    continue;
                }
                std::cout << "RESPOND 2 end" << endl;
            }
        
        }
    }
}