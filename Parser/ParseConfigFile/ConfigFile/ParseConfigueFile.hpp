#ifndef PARSECONFIGUEFILE_HPP
#define PARSECONFIGUEFILE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../ServerConfig/ServerConfig.hpp"
#include "../../Header/HeaderFiles.hpp"


class clsParseConfigueFile
{
public:
	enum eKeyBlock { SERVER_BLOCK, UNKNOWN};

private:
	std::vector<clsServerConfig>	_servers;
	clsParse<TokenType>				_Parse;
	HttpError						_ERROR;

	eKeyBlock searchBlock(const std::string& WORD);
	void addServer(const clsServerConfig& serve);
	bool BlockServer(s_parse_context	&ctx);
	void    initBlockServer();

public:
	clsParseConfigueFile(clsParse<TokenType> &Parse);

	bool ParseConfigue();
	std::vector<clsServerConfig> &getServers();
	HttpError					getError();
};

#endif
