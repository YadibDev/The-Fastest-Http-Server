#include "ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
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

    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "========== [ SERVER " << i + 1 << " ] ==========" << std::endl;
        
        std::vector<sockaddr_in> listens = servers[i].getListens();
        for (size_t j = 0; j < listens.size(); ++j) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(listens[j].sin_addr), ip, INET_ADDRSTRLEN);
            std::cout << "  - Listening on: " << ip << ":" << ntohs(listens[j].sin_port) << std::endl;
        }

        std::cout << "  - Root: " << servers[i].getRoot() << std::endl;
        std::cout << "  - Max Body Size: " << servers[i].getMaxBodySize() << std::endl;

        std::map<short, stErrorPagedata> errPages = servers[i].getErrorPages();
        if (!errPages.empty()) {
            std::cout << "  - Error Pages: ";
            std::map<short, stErrorPagedata>::const_iterator it;
            for (it = errPages.begin(); it != errPages.end(); ++it) {
                std::cout << "[" << it->first << " -> " << it->second.uri << "] ";
            }
            std::cout << std::endl;
        }

        std::vector<clsLocation> prefixLocs = servers[i].getLocationPrefix();
        std::cout << "  - Prefix Locations (" << prefixLocs.size() << "):" << std::endl;
        for (size_t k = 0; k < prefixLocs.size(); ++k) {
            std::cout << "      * Path: " << prefixLocs[k].getLocationData().uri << std::endl;
            std::cout << "        Methods: "; printMethods(prefixLocs[k].getAllowMethods());
            std::cout << "\n        Autoindex: " << (prefixLocs[k].getAutoIndex() ? "ON" : "OFF") << std::endl;
            if (!prefixLocs[k].getRoot().empty())
                std::cout << "        Root: " << prefixLocs[k].getRoot() << std::endl;
        }

        std::vector<clsLocation> exactLocs = servers[i].getLocationExact();
        if (!exactLocs.empty()) {
            std::cout << "  - Exact Locations (" << exactLocs.size() << "):" << std::endl;
            for (size_t k = 0; k < exactLocs.size(); ++k) {
                std::cout << "      * Path: " << exactLocs[k].getLocationData().uri << std::endl;
                std::cout << "        Upload Store: " << exactLocs[k].getUploadStore() << std::endl;
                std::cout << "        Methods: "; printMethods(exactLocs[k].getAllowMethods());
                std::cout << "\n        Autoindex: " << (exactLocs[k].getAutoIndex() ? "ON" : "OFF") << std::endl;
                if (!exactLocs[k].getRoot().empty())
                    std::cout << "        Root: " << exactLocs[k].getRoot() << std::endl;
                
            }
        }
        std::cout << std::endl;
    }

    return 0;
}