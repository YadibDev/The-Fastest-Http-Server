#include "ParseConfigueFile.hpp"

clsParseConfigueFile::clsParseConfigueFile(clsParse<TokenType> Parse)
    : _Parse(Parse)
{}

clsParseConfigueFile::eKeyBlock clsParseConfigueFile::searchBlock(const std::string& WORD)
{
    if (WORD == "server")
        return SERVER_BLOCK;
    throw HttpError(400, "Unknown block: " + WORD);
}

void clsParseConfigueFile::addServer(const clsServerConfig& serve)
{
    _servers.push_back(serve);
}

HttpError clsParseConfigueFile::BlockServer()
{
    clsServerConfig ServerConfig(_Parse);
    HttpError Error = ServerConfig.parseBlockServer(); // افترض parseBlockServer() يعيد HttpError

    if (Error._codeStatus != 200)
        return Error;

    addServer(ServerConfig);
    return Error;
}

bool clsParseConfigueFile::ParseConfigue()
{
    while (_Parse.peek().type != TOKEN_EOF) {
        if (_Parse.peek().type != TOKEN_WORD) {
            _error.setStatus(400, "Syntax Error: Expected block name");
            return false;
        }

        std::string blockName = _Parse.peek().value;
        eKeyBlock blockType;

        blockType = searchBlock(blockName);
        
        if (blockType == SERVER_BLOCK) {
            HttpError err = BlockServer();
            if (err.isError()) {
                _error = err;
                return false;
            }
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
