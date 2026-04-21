#include "clsFlow.hpp"

void clsFlow::_initializeStatics()
{
    HelperFunctions::StoredDefaultType();
    HelperFunctions::StoredBodys();
    HelperFunctions::StoredMessage();
    HelperFunctions::StoredMessage();
    // signal(SIGINT, function);
}

void clsFlow::_createBlocksServers()
{
    int fd = open("configs/default.conf", O_RDONLY);
    if (fd == -1)
    {
        throw std::runtime_error("fail to open config file");
    }
    std::string configeData;
    configeData.resize(10000);
    int sizeRead = read(fd, &configeData[0], 9999);
    if (sizeRead == 0 || read(fd, &configeData[9999], 1) > 0)
        throw std::runtime_error("Error can be on of the following reasons :\n- config file too large \n- empty file\n");
    configeData.resize(sizeRead);

    LexerConfig<TokenType> lexerConfig(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);
    lexerConfig.addSeparatorToken('{', TOKEN_LBRACE);
    lexerConfig.addSeparatorToken('}', TOKEN_RBRACE);
    lexerConfig.addSeparatorToken(';', TOKEN_SEMICOLON);
    lexerConfig.addCommentRule("#", "\n");
    lexerConfig.addWithSpace(" \t\n");
    static GenericLexer<TokenType> lexer(configeData, lexerConfig);
    static std::vector<Token<TokenType> > Lexer = lexer.tokenize();
    static clsParse<TokenType> Data(Lexer, TOKEN_EOF);
    static clsParseConfigueFile ConfigFile(Data);

    ConfigFile.ParseConfigue();
    if (ConfigFile.getServers().size() == 0)
    {
        std::cout << "Error\n"
                  << ConfigFile.getError().getMsgError() << std::endl;
        std::cout << ConfigFile.getError().getCodeStatus() << std::endl;
        throw std::runtime_error("0 block server\n");
    }

    _allBlocks = &(ConfigFile.getServers());
}

void clsFlow::_createServers()
{
    _totalServers = 0;

    for (size_t i = 0; i < _allBlocks->size(); i++)
    {
        clsServerSock serv;
        try
        {
            serv.buildSockets(_allBlocks->at(i).getListens());
            _allServers.push_back(serv);
            serv.disableCloseAtEnd();
            _totalServers++;
        }
        catch (std::exception &e)
        {
            std::cout << "---- Warning ----\n---- can't build server Number " << i + 1 << " because :" << std::endl;
            std::cout << "---- " << e.what() << std::endl;
        }
    }
    if (_totalServers == 0)
        throw std::runtime_error("Error\nthere is no server or there is a problem in all the servers");
    // debug
    std::cout << "total servers created [" << _totalServers << "]" << std::endl;
}

void clsFlow::_initializeDataBase()
{
    _clientsArr = new clsClient[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
        _clientsAvailable.push(i);
}

short clsFlow::_getClient()
{
    if (_clientsAvailable.size() == 0)
        return -1;
    short id = _clientsAvailable.top();
    _clientsAvailable.pop();
    return id;
}

void clsFlow::_pushClient(short clientFd)
{
    short index = clientIdByFd[clientFd];
    clientIdByFd.erase(clientFd);
    // _clientsArr[index].freeRessources()
    _clientsAvailable.push(index);
}

void clsFlow::_registerServersSockets()
{
    int Erase = 0;
    for (size_t i = 0; i < _allServers.size(); i++)
    {
        try
        {
            clsServerSock &server = _allServers[i - Erase];
            _epoll.addServerSockets(server);
        }
        catch (...)
        {
            Erase++;
            _allServers.erase(_allServers.begin() + i);
        }
    }
    if (_allServers.size() == 0)
        throw("Error\ncan't register all servers sockets");
    std::cout << "Register Server Sockets by success\n";
}

clsFlow::clsFlow()
{
    _initializeStatics();
    _createBlocksServers();
    _createServers();
    _initializeDataBase();
    _registerServersSockets();
}

bool clsFlow::_eventsEroorHandle(epoll_event &client)
{
    if ((client.events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP)))
    {
        int fd = client.data.fd;
        if (client.events & EPOLLRDHUP)
            std::cout << "EPOLLRDHUP" << std::endl;
        else if (client.events & EPOLLERR)
            std::cout << "EPOLLERR" << std::endl;
        else
            std::cout << "EPOLLHUP" << std::endl;
        // if (getsockname)
            // ClientsLinker.removeClient(fd);
        _pushClient(fd);
        return true;
    }
    return false;
}

clsFlow::fdTypes clsFlow::_fdType(int fd)
{
    sockaddr_in addr;
    socklen_t size;

    std::memset(&addr, 0, sizeof(addr));
    if (getsockname(fd, reinterpret_cast<sockaddr *>(&addr), &size) == -1)
    {
        return PIPE;
    }
    else if (getpeername(fd, reinterpret_cast<sockaddr *>(&addr), &size) == -1)
    {
        return SERVER_SOCK;
    }
    return CLIENT_SOCK;
}

// bool clsFlow::tryPushClient()
// {

// }

void clsFlow::EventLoop()
{
    int nFds = 0;
    while ((nFds = _epoll.tryPollNewClients(_clientsEvents, EVENTS_MAX, -1)))
    {
        for (int i = 0; i < nFds; i++)
        {
            if (_eventsEroorHandle(_clientsEvents[i]))
                continue ;
            else
            {
                if (_fdType(_clientsEvents[i].data.fd) == CLIENT_SOCK)
                    std::cout << "client sock" << endl;
                else if (_fdType(_clientsEvents[i].data.fd) == SERVER_SOCK)
                {
                    std::cout << "SERVER_SOCK" << endl;
                    int fd[2];
                    pipe(fd);
                    std::cout << _fdType(accept(_clientsEvents[i].data.fd, NULL, NULL)) << endl;;
                    std::cout << _fdType(fd[0]) << endl;;
                }
                else
                    std::cout << "PIPE" << endl;
            }
        }
    }
    throw std::runtime_error("Error\n epoll system call fail");
}