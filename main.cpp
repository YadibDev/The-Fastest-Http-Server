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
    int fd = open("config.conf", O_RDONLY);
    ssize_t Size = 1024;

    HelperFunctions::ReadData(fd, Data, Size);

    LexerConfig<TokenType> cfg(TokenType::TOKEN_WORD, TokenType::TOKEN_EOF, TokenType::TOKEN_NULL);

    GenericLexer<TokenType> Lexer(Data, cfg);

    std::vector< Token<TokenType> > Tokens = Lexer.tokenize();

    clsParse Parse(Tokens, TokenType::TOKEN_EOF);
    clsParseConfigueFile configFile(Parse);

    
    
    return 0;
}