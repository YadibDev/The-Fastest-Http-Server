#include "Parser/ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "Parser/ParseRequest/Request/Request.hpp"
#include "Parser/RequestHandler/ProcessRequestHandler.hpp"
#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>
#include <iomanip>

void printMethods(short methods) {
    if (methods & 1) std::cout << "GET ";
    if (methods & 2) std::cout << "POST ";
    if (methods & 4) std::cout << "DELETE ";
}

int main() {
    std::string Data;
    int fd = open("config-files/default.conf", O_RDONLY);
    if (fd == -1) {
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
    std::vector< Token<TokenType> > Tokens = Lexer.tokenize();
    clsParse<TokenType> Parse(Tokens, TOKEN_EOF);

    clsParseConfigueFile configFile(Parse);

    if (!configFile.ParseConfigue()) {
        std::cout << "\033[1;31mParsing Error: " << configFile.getError().getMsgError() << "\033[0m" << std::endl;
        return 1;
    }

    std::vector<clsServerConfig> servers = configFile.getServers();
    std::cout << "\033[1;32mTotal Servers Parsed: " << servers.size() << "\033[0m\n" << std::endl;

    std::string Requestest = "POST /uploads/test.txt HTTP/1.1\r\nHost: example.com\r\n\r\n";
    clsRequest request;

    request.parse(Requestest);

    RequestHandler handler;
    ProcessRequestHandler::processRequest(request, servers[0], handler);

    std::cout << "Physical Path: " << handler.getPhysicalPath() << std::endl;
    std::cout << "Autoindex: " << (handler.getAutoIndex() ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Allowed Methods: ";
    printMethods(handler.getAllowMethod());
    std::cout << std::endl;
    std::cout << "Query: " << handler.getQuery() << std::endl;
    std::cout << "Method: " << handler.getMethod() << std::endl;
    std::cout << "Path CGI: " << handler.getPathCgi() << std::endl;
    std::cout << "Upload Store: " << handler.getUploadStore() << std::endl;
    std::cout << "Return Data: " << handler.getReturnData().code << " " << handler.getReturnData().value << std::endl;
    return 0;
}