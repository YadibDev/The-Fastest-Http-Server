#ifndef PARSECONFIGUEFILE_HPP
#define PARSECONFIGUEFILE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../ServerConfig/ServerConfig.hpp"
#include "../../Utils/Lexer.hpp"
#include "../../Utils/HttpError.hpp"


class clsParseConfigueFile
{
public:
	enum eKeyBlock { SERVER_BLOCK, UNKNOWN};

private:
	std::vector<clsServerConfig>	_servers;
	clsParse<TokenType>				&_Parse;
	HttpError						&_ERROR;

	eKeyBlock searchBlock(const std::string& WORD);
	void addServer(const clsServerConfig& serve);
	bool BlockServer();

public:
	clsParseConfigueFile(clsParse<TokenType> Parse);

	bool ParseConfigue();
	std::vector<clsServerConfig> getServers() const;
	HttpError					getError();
};

#endif
