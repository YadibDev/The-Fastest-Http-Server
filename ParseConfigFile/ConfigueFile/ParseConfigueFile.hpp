#define PARSECONFIGUEFILE_HPP
#ifndef PARSECONFIGUEFILE_HPP

#include <iostream>
#include <string>
#include "../ParseServerConfig.hpp"
#include "../../Utils/HttpError.hpp"
#include "../../Utils/Lexer.hpp"

class clsParseConfigueFile
{
	static enum	eKeyBlock {SERVER_BLOCK};
	std::vector<clsServerConfig>	_servers;
	clsParse<TokenType>			_Parse;

	enum eKeyBlock searchBlock(std::string WORD)
	{
		switch (WORD)
		{
			case "server":
				return eKeyBlock::SERVER_BLOCK;
		
			default:
				break;
		}
	}

	void addServer(const clsServerConfig& serve) {
		_servers.push_back(serve);
	}

	HttpError BlockServer()
	{
		clsServerConfig ServerConfig(_Parse);
		HttpError Error = ServerConfig.parseBlockServer()._codeStatus
		if (Error._codeStatus != 200)
			return Error;
		addServer(ServerConfig);
		return Error;
	}

	public:
		clsParseConfigueFile (clsParse<TokenType> Parse) : _Parse(Parse);
		HttpError ParseConfigue()
		{
			while (_Parse.peek().type != TokenType::TOKEN_EOF)
			{
				if (searchBlock(_Parse.peek().value) == eKeyBlock::SERVER_BLOCK)
					
				else if (_Parse.peek().value == "\n")
				{
					_Parse.advance();
					continue ;
				}
				else
					return HttpError(400, "syntax Error in " + _Parse.peek().value);
			}
			return HttpError(200, "");
		}
}

#endif