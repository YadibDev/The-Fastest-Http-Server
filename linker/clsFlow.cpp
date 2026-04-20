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
    static std::vector<Token<TokenType> > Lexer = lexer.tokenize();
    static clsParse<TokenType> Data(Lexer, TOKEN_EOF);
    static clsParseConfigueFile ConfigFile(Data);

    ConfigFile.ParseConfigue();
    if (ConfigFile.getServers().size() == 0)
    {
        std::cout << "Error\n" << ConfigFile.getError().getMsgError() << std::endl;
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
            std::cout << "---- " <<  e.what() << std::endl;
        }
    }
    if (_totalServers == 0)
        throw std::runtime_error("Error\nthere is no server or there is a problem in all the servers");
    // debug
    std::cout << "total servers created [" << totalServers << "]" << std::endl; 
}

