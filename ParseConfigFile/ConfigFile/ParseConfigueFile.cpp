#include "ParseConfigueFile.hpp"

clsParseConfigueFile::clsParseConfigueFile(clsParse<TokenType> &Parse)
    : _Parse(Parse)
{
    
}

clsParseConfigueFile::eKeyBlock clsParseConfigueFile::searchBlock(const std::string& WORD)
{
    std::cout << "|" << WORD << "|" << std::endl;
    if (WORD == "server")
        return SERVER_BLOCK;
    return UNKNOWN;
}

void clsParseConfigueFile::addServer(const clsServerConfig& serve)
{
    _servers.push_back(serve);
}

bool    clsParseConfigueFile::BlockServer()
{
    clsServerConfig server();
    if (!server.parseBlockServer())
    {
        _error = server.getError();
        return false;
    }
    addServer(server);
    return true;
}

bool clsParseConfigueFile::ParseConfigue()
{
    s_parse_context	ctx(_Parse, _ERROR);

    while (_Parse.peek().type != TOKEN_EOF) {
        if (_Parse.peek().type != TOKEN_WORD) {
            _error.setStatus(400, "Syntax Error: Expected block name");
            return false;
        }

        std::string blockName = _Parse.peek().value;
        eKeyBlock blockType;

        blockType = searchBlock(blockName);
        
        if (blockType == SERVER_BLOCK) {
            
            if (!BlockServer())
                return false;
            
        }
        else
            return (_error.setStatus(400, "Unknown block: " + blockName), false);
    }
    return true;
}


std::vector<clsServerConfig> clsParseConfigueFile::getServers() const
{
    return (_servers);
}


HttpError					clsParseConfigueFile::getError()
{
    return _error;
}