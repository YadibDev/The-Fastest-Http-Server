#include "ParseConfigueFile.hpp"

clsParseConfigueFile::clsParseConfigueFile(clsParse<TokenType> &Parse)
    : _Parse(Parse)
{
    
}

clsParseConfigueFile::eKeyBlock clsParseConfigueFile::searchBlock(const std::string& WORD)
{
    if (WORD == "server")
        return SERVER_BLOCK;
    return UNKNOWN;
}

void clsParseConfigueFile::addServer(const clsServerConfig& serve)
{
    _servers.push_back(serve);
}

bool    clsParseConfigueFile::BlockServer(s_parse_context	&ctx)
{
    clsServerConfig server(ctx);
    if (!server.parseBlockServer())
    {
        _ERROR = ctx.error;
        return false;
    }
    addServer(server);
    return true;
}

void    clsParseConfigueFile::initBlockServer()
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        _servers[i].initUri();
    }
}

bool clsParseConfigueFile::ParseConfigue()
{
    s_parse_context	ctx(_Parse, _ERROR);

    ConfigDirectiveParser::skipWhitespace(_Parse);

    while (_Parse.peek().type != TOKEN_EOF) {
        if (_Parse.peek().type != TOKEN_WORD) {
            _ERROR.setStatus(400, "Syntax Error: Expected block name");
            return false;
        }

        std::string blockName = _Parse.peek().value;
        eKeyBlock blockType;

        blockType = searchBlock(blockName);
        
        if (blockType == SERVER_BLOCK)
        {
            
            if (!BlockServer(ctx))
                return false;
            
        }
        else
            return (_ERROR.setStatus(400, "Unknown block: " + blockName), false);
    }
    initBlockServer();
    return true;
}

std::vector<clsServerConfig> &clsParseConfigueFile::getServers() { return (_servers); }

HttpError					clsParseConfigueFile::getError() { return _ERROR; }