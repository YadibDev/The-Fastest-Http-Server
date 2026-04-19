
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

int main()
{
    HelperFunctions::StoredDefaultType();
    HelperFunctions::StoredBodys();
    HelperFunctions::StoredMessage();
    HelperFunctions::StoredMessage();
    signal(SIGINT, function);
    int fd = open("configs/default.conf", O_RDONLY);
    std::string configeData;
    configeData.resize(1025);

    read(fd, &configeData[0], 1024);

    // fd, configeData, 1024

    LexerConfig<TokenType> lexerConfig(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);

    lexerConfig.addSeparatorToken('{', TOKEN_LBRACE);
    lexerConfig.addSeparatorToken('}', TOKEN_RBRACE);
    lexerConfig.addSeparatorToken(';', TOKEN_SEMICOLON);

    lexerConfig.addCommentRule("#", "\n");
    lexerConfig.addWithSpace(" \t\n");

    GenericLexer<TokenType> lexer(configeData, lexerConfig);

    std::vector<Token<TokenType> > Lexer = lexer.tokenize();

    clsParse<TokenType> Data(Lexer, TOKEN_EOF);
    clsParseConfigueFile ConfigueFile(Data);

    ConfigueFile.ParseConfigue();
    if (!ConfigueFile.getServers().size())
    {
        std::cout << ConfigueFile.getError().getMsgError() << std::endl;
        std::cout << ConfigueFile.getError().getCodeStatus() << std::endl;
        std::cout << "Block Server ZERO\n"
                  << std::endl;
        return 1;
    }

    clsServerConfig Block = ConfigueFile.getServers()[0];

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
            if ((ClientBuffer[i].events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP)))
            {
                if (ClientBuffer[i].events & EPOLLRDHUP)
                    std::cout << "EPOLLRDHUP" << std::endl;
                else if (ClientBuffer[i].events & EPOLLERR)
                    std::cout << "EPOLLERR" << std::endl;
                else
                    std::cout << "EPOLLHUP" << std::endl;
                std::cout << "Fd " << fd << std::endl;
                ClientsLinker.removeClient(fd);
                std::cout << "Removed removed\n";
                continue;
            }
            else if ((ClientBuffer[i].events & EPOLLIN) == EPOLLIN)
            {
                newClient = server.tryAcceptNewClient(fd, &addr);
                

                if (newClient == -1)
                {
                    if (newClient == -1)
                        std::cerr << "Accept Fail" << std::endl;
                    continue ;
                }
                else if (newClient > 0)
                {
                    // flow of accept new client
                    std::cout << "accepted\n";
                    std::cout << newClient << std::endl;
                    ClientsLinker.insertClient(newClient, addr, Block);
                    epoll.addClient(newClient, EPOLLIN);
                }
                else
                {
                    newClient = fd;

                    clsClient &client = ClientsLinker.GetClientAt(newClient);

                    client.ProcessRequest();
                    if (client.GetState() == CONNECTION_CLOSED)
                    {
                        std::cout << "Removed\n";
                        ClientsLinker.removeClient(newClient);
                        continue ;
                    }
                    if (client.GetState() == START_RESPOND)
                    {
                        epoll.changeAbility(newClient, EPOLLOUT);
                    }
                }
            }
            else if ((ClientBuffer[i].events & EPOLLOUT) == EPOLLOUT)
            {
                newClient = fd;

                clsClient &client = ClientsLinker.GetClientAt(newClient);

                client.ProcessRespond();
                if (client.GetState() == BEGIN)
                    epoll.changeAbility(newClient, EPOLLIN);
                else if (client.GetState() == CONNECTION_CLOSED)
                {
                    std::cout << "Removed\n";
                    ClientsLinker.removeClient(newClient);
                    std::cout << newClient << std::endl;
                    continue;
                }
            }
        }
    }
}