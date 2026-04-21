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
        throw std::runtime_error("Error can be on of the following reasons :\n- config file too large \n- empty file");
    configeData.resize(sizeRead);

    LexerConfig<TokenType> lexerConfig(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);
    lexerConfig.addSeparatorToken('{', TOKEN_LBRACE);
    lexerConfig.addSeparatorToken('}', TOKEN_RBRACE);
    lexerConfig.addSeparatorToken(';', TOKEN_SEMICOLON);
    lexerConfig.addCommentRule("#", "\n");
    lexerConfig.addWithSpace(" \t\n");
    static GenericLexer<TokenType> lexer(configeData, lexerConfig);
    static std::vector<Token<TokenType>> Lexer = lexer.tokenize();
    static clsParse<TokenType> Data(Lexer, TOKEN_EOF);
    static clsParseConfigueFile ConfigFile(Data);

    ConfigFile.ParseConfigue();
    if (ConfigFile.getServers().size() == 0)
    {
        std::cout << "Error\n"
                  << ConfigFile.getError().getMsgError() << std::endl;
        std::cout << ConfigFile.getError().getCodeStatus() << std::endl;
        throw std::runtime_error("0 block server");
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

void clsFlow::_pushClient(short clientId)
{
    _clientsAvailable.push(clientId);
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
    flow._initializeStatics();
    flow._createBlocksServers();
    flow._createServers();
    flow._initializeDataBase();
    flow._registerServersSockets();
}

void clsFlow::EventLoop(epoll_event &client)
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
        
        continue;
    }
}

void clsFlow::clsFlow()
{
    int nFds = 0;
    while ((nFds = epoll.tryPollNewClients(_clientsEvents, EVENTS_MAX, -1)))
    {
        for (int i = 0; i < n; i++)
        {
        }
    }
    throw std::runtime_error("Error\n epoll system call fail");
}