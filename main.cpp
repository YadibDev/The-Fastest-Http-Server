#include "ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"

// enum TokenType {
//     TOKEN_NULL,
//     TOKEN_WORD,
//     TOKEN_LBRACE,
//     TOKEN_RBRACE,
//     TOKEN_SEMICOLON,
//     TOKEN_FASSILA,
//     TOKEN_JOUJNO9ATE,
//     TOKEN_EOF
// };

int main()
{
    std::string Data;
    int fd = open("config-files/default.conf", O_RDONLY);
    ssize_t Size = 1326;

    if (fd == -1)
        return 1;
    
    HelperFunctions::ReadData(fd, Data, Size);

    LexerConfig<TokenType> cfg(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);
    cfg.addCommentRule("#", "\n");
    cfg.addSeparatorToken('{', TOKEN_LBRACE);
    cfg.addSeparatorToken('}', TOKEN_RBRACE);
    cfg.addSeparatorToken(';', TOKEN_SEMICOLON);
    cfg.addSeparatorToken('\n', TOKEN_JOUJNO9ATE);

    
    cfg.addWithSpace(" \t");
    GenericLexer<TokenType> Lexer(Data, cfg);

    std::vector< Token<TokenType> > Tokens = Lexer.tokenize();

    clsParse<TokenType> Parse(Tokens, TOKEN_EOF);

    // if (Tokens.empty())
    //     std::cout << "HI\n";
    clsParseConfigueFile configFile(Parse);

    if (!configFile.ParseConfigue())
    {
        std::cout << configFile.getError().getMsgError() << std::endl;
        return (1);
    }
    
    std::cout << "Total Servers " <<  configFile.getServers().size() << std::endl;
    std::cout << configFile.getServers()[0].getListens()[0].sin_addr.s_addr << std::endl;
    std::cout << configFile.getServers()[0].getListens()[0].sin_port << std::endl;
    std::cout << configFile.getServers()[0].getRoot() << std::endl;
    
    
    return 0;
}