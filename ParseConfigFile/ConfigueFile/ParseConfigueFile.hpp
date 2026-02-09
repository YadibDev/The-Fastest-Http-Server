#define CONFIGUEFILE_HPP
#ifndef CONFIGUEFILE_HPP

#include <iostream>
#include <string>
#include "../ParseServerConfig.hpp"
#include "../../Utils/Lexer.hpp"

class clsParseConfigueFile
{
	std::vector<ServerConfig>	_servers;
	clsParse<TokenType>			_Parse;

	public:
		clsParseConfigueFile (clsParse<TokenType> Parse) : _Parse(Parse);
		std::vector<ServerConfig> ParseConfigue()
		{
			
			while (_Parse.peek().)
		}
}

#endif