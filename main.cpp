#include "ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"


int main()
{
    std::string Data;
    int fd = open("config.conf", O_RDONLY);
    ssize_t Size = 1024;

    HelperFunctions::ReadData(fd, Data, Size);

    LexerConfig<TokenType> cfg(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);

    GenericLexer<TokenType> Lexer(Data, cfg);

    std::vector< Token<TokenType> > Tokens = Lexer.tokenize();

    clsParse<TokenType> Parse(Tokens, TOKEN_EOF);
    clsParseConfigueFile configFile(Parse);

    
    
    return 0;
}