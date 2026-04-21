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
            serv.setBlock(&_allBlocks->at(i));
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

void clsFlow::_freeClient(short clientFd)
{
    short index = _clientIdByFd[clientFd];
    _clientIdByFd.erase(clientFd);
    // _clientsArr[index].freeRessources()
    _clientsAvailable.push(index);
}

void clsFlow::_registerServersSockets()
{
    int Erase = 0;
    for (size_t i = 0; i < _allServers.size() + Erase; i++)
    {
        try
        {
            clsServerSock &server = _allServers[i - Erase];
            _epoll.addServerSockets(server);
        }
        catch (...)
        {
            _allServers.erase(_allServers.begin() + (i - Erase));
            Erase++;
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
        _freeClient(fd);
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
    else
        return SERVER_SOCK;
}

bool clsFlow::_insertClient(int newClient, sockaddr_in &addr, clsServerConfig *block)
{
    int blockId = this->_getClient();
    if (blockId == -1)
        return false;
    clsClient &client = _clientsArr[blockId];
    _clientIdByFd[newClient] = blockId;

    if (_epoll.addClient(newClient, EPOLLIN) == false)
        return false; // watch by epoll
    client.initializeClient(addr, newClient, block);
    return true;
}

void clsFlow::_clientProcess(int fd, uint32_t event)
{
    int index = _clientIdByFd[fd];
    clsClient &client = _clientsArr[index];
    client.ProcessBoth(event);
    const clinetState &status = client.GetState();

    if (status == CONNECTION_CLOSED)
        _freeClient(fd);
    else if (status == RESPOND_MODE)
    {
        _epoll.changeAbility(fd, EPOLLOUT);
    }
    else if (status == BEGIN)
    {
        _epoll.changeAbility(fd, EPOLLIN);
    }
}

void clsFlow::_newClientProcess(int serverFd)
{
    for (size_t i = 0; i < _allServers.size(); i++)
    {
        clsServerSock &server = _allServers[i];
        sockaddr_in addr;

        int newClient = server.tryAcceptNewClient(serverFd, &addr);
        if (newClient > 0)
        {
            _insertClient(newClient, addr, server.getBlock());
            break ;
        }
        else if (newClient == -1)
            break;
    }
}

void clsFlow::_flowProcess(int fd, fdTypes &TypeFd, int indexEvent)
{
    if (TypeFd == CLIENT_SOCK)
        _clientProcess(fd, _clientsEvents[indexEvent].events);
    else if (TypeFd == SERVER_SOCK)
        _newClientProcess(fd);

    // else if (TypeFd == PIPE)
        // cgi work here
}

void clsFlow::EventLoop()
{
    int nFds = 0;
    fdTypes TypeFd;
    while ((nFds = _epoll.tryPollNewClients(_clientsEvents, EVENTS_MAX, -1)))
    {
        for (int i = 0; i < nFds; i++)
        {
            if (_eventsEroorHandle(_clientsEvents[i]))
                continue ;
            else
            {
                int fd = (_clientsEvents[i].data.fd);
                if (_clientIdByFd.count(fd))
                    TypeFd = CLIENT_SOCK;
                else
                    TypeFd = _fdType(fd);

                _flowProcess(fd, TypeFd, i);
            }
        }
    }
    throw std::runtime_error("Error\n epoll system call fail");
}