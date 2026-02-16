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

HttpError clsParseConfigueFile::ParseConfigue()
{
    while (_Parse.peek().type != TokenType::TOKEN_EOF)
    {
        const std::string& value = _Parse.peek().value;

        if (value == "server")
        {
            HttpError err = BlockServer();
            if (err._codeStatus != 200)
                return err;
        }
        else if (value == "\n")
        {
            _Parse.advance();
            continue;
        }
        else
        {
            return HttpError(400, "Syntax Error in: " + value);
        }
    }

    return HttpError(200, "");
}

std::vector<clsServerConfig> clsParseConfigueFile::getServers() const
{
    return _servers;
}
