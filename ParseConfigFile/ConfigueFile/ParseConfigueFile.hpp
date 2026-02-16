#ifndef PARSECONFIGUEFILE_HPP
#define PARSECONFIGUEFILE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../ServerConfig/ParseServerConfig.hpp"
#include "../../Utils/HttpError.hpp"
#include "../../Utils/Lexer.hpp"

class clsParseConfigueFile
{
public:
    enum eKeyBlock { SERVER_BLOCK };

private:
    std::vector<clsServerConfig> _servers;
    clsParse<TokenType> _Parse;

    eKeyBlock searchBlock(const std::string& WORD);
    void addServer(const clsServerConfig& serve);
    HttpError BlockServer();

public:
    clsParseConfigueFile(clsParse<TokenType> Parse);

    HttpError ParseConfigue();
    std::vector<clsServerConfig> getServers() const;
};

#endif
