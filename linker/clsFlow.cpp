#include "clsFlow.hpp"

void clsFlow::_initializeStatics()
{
	HelperFunctions::StoreVarConst();
	HelperFunctions::StoredDefaultType();
	HelperFunctions::StoredBodys();
	HelperFunctions::StoredMessage();
	HelperFunctions::StoredMessage();
	// signal(SIGINT, function);
}

void clsFlow::_createBlocksServers(const char *configFile)
{
	int fd = open(configFile, O_RDONLY);
	if (fd == -1)
	{
		throw std::runtime_error("fail to open config file");
	}
	std::string configeData;
	configeData.resize(10000);
	int sizeRead = read(fd, &configeData[0], 9999);
	if (sizeRead == 0 || read(fd, &configeData[9999], 1) > 0)
	{
		close(fd);
		throw std::runtime_error("Error can be on of the following reasons :\n- config file too large \n- empty file\n");
	}
	close(fd);
	configeData.resize(sizeRead);

	static LexerConfig<TokenType> lexerConfig(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);
	lexerConfig.addSeparatorToken('{', TOKEN_LBRACE);
	lexerConfig.addSeparatorToken('}', TOKEN_RBRACE);
	lexerConfig.addSeparatorToken(';', TOKEN_SEMICOLON);
	lexerConfig.addCommentRule("#", "\n");
	lexerConfig.addWithSpace(" \t\n");
	static GenericLexer<TokenType> lexer(configeData, lexerConfig);
	static std::vector<Token<TokenType> > Lexer = lexer.tokenize();
	static clsParse<TokenType> Data(Lexer, TOKEN_EOF);
	static clsParseConfigueFile ConfigFile(Data);

	ConfigFile.ParseConfigue();
	if (ConfigFile.getServers().size() == 0)
	{
		std::cout << "Error\n"
				  << ConfigFile.getError().getMsgError() << std::endl;
		std::cout << ConfigFile.getError().getCodeStatus() << std::endl;
		throw std::runtime_error("0 block server\n");
	}

	_allBlocks = &(ConfigFile.getServers());
}

void clsFlow::_createServers()
{
	_totalServers = 0;
	for (size_t i = 0; i < _allBlocks->size(); i++)
	{
		clsServerSock serv;
		try
		{
			serv.buildSockets(_allBlocks->at(i).getListens());
			serv.setBlock(&_allBlocks->at(i));
			_allServers.push_back(serv);
			_totalServers++;
		}
		catch (std::exception &e)
		{
			std::cout << "---- Warning ----\n---- can't build server Number " << i + 1 << " because :" << std::endl;
			std::cout << "---- " << e.what() << std::endl;
		}
	}
	if (_totalServers == 0)
		throw std::runtime_error("Error\nthere is no server or there is a problem in all the servers");
}

void clsFlow::_initializeDataBase()
{
	_clientsArr = new clsClient[maxClient];
	for (int i = 0; i < maxClient; i++)
		_clientsAvailable.push(i);
}

short clsFlow::_getClient()
{
	if (_clientsAvailable.size() == 0)
		return -1;
	short id = _clientsAvailable.top();
	_clientsAvailable.pop();
	return id;
}

void clsFlow::_freeClient(short clientFd)
{
	short index = _clientIdByFd[clientFd];
	_clientIdByFd.erase(clientFd);
	_popPipe(_clientsArr[index].getPipeCgi());
	_clientsArr[index].freeRessources();
	_clientsAvailable.push(index);
}

void clsFlow::_registerServersSockets()
{
	std::vector<clsServerSock>::iterator it = _allServers.begin();
	std::vector<clsServerSock>::iterator end = _allServers.end();

	while (it != end && _allServers.size())
	{
		try
		{
			if (_epoll.addServerSockets(*it, EPOLLIN) == false)
				throw std::runtime_error("Error\nservers can't add his socket to epoll");
			++it;
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
			it->freeAllSockets();
			it = _allServers.erase(it);
		}
	}
	// std::cout << "out\n" << std::endl;
	if (_allServers.size() == 0)
		throw(std::runtime_error("Error\ncan't register all servers sockets"));

	for (size_t i = 0; i < _allServers.size(); i++)
		_allServers[i].enableCloseAtEnd();

	std::cout << "Register Server Sockets by success\n";
}

clsFlow::clsFlow(const char *configFile, long maxClient) : maxClient(maxClient)
{
	_clientsArr = NULL;
	_initializeStatics();
	_createBlocksServers(configFile);
	_createServers();
	_registerServersSockets();
	_initializeDataBase();
}

bool clsFlow::_eventsEroorHandle(epoll_event &client, fdTypes &TypeFd)
{
	if (client.events & (EPOLLERR | EPOLLHUP))
	{
		int fd = client.data.fd;

		if (TypeFd == PIPE)
		{
			int index = _IdByPipe[fd];
			if (client.events & EPOLLERR)
			{
				_clientsArr[index].forceStopCgi();
				_popPipe(fd);
			}
			else if (client.events & EPOLLHUP)
			{
				if (_clientsArr[index].monitorCgi())
					_popPipe(fd);
			}
		}
		else if (TypeFd == CLIENT_SOCK)
		{
			if (client.events & (EPOLLERR))
			{
				_freeClient(fd);
			}
			else if (client.events & EPOLLHUP)
			{
				_clientsArr[index].peerClosed();
				return false;
			}
		}
		return true;
	}
	return false;
}

bool clsFlow::_insertClient(int newClient, sockaddr_in &addr, clsServerConfig *block, uint16_t port)
{
	int blockId = this->_getClient();
	if (blockId == -1)
	{
		close(newClient);
		return false;
	}
	clsClient &client = _clientsArr[blockId];
	_clientIdByFd[newClient] = blockId;

	if (_epoll.addClient(newClient, EPOLLIN) == false)
		return false;
	client.initializeClient(addr, newClient, block, port);
	return true;
}

void clsFlow::_pushPipe(short pipe, short indexClient)
{
	if (HelperFunctions::changeFileToNonBlocking(pipe) == -1)
	{
		std::cout << "========> fcntl fail add pipe <=========\n"
				  << std::endl;
		return;
	}
	if (_epoll.addClient(pipe, EPOLLIN) == false)
		return;
	_IdByPipe[pipe] = indexClient;
}

void clsFlow::_popPipe(short pipe)
{
	_IdByPipe.erase(pipe);
}

void clsFlow::_clientProcess(int fd, uint32_t event)
{
	short index = _clientIdByFd[fd];
	clsClient &client = _clientsArr[index];

	client.ProcessBoth(event);
	const clinetState &status = client.GetState();

	if (status == BEGIN || status == CONNECTION_CLOSED)
	{
		client.logs();
	}

	if (status == CONNECTION_CLOSED)
	{
		_freeClient(fd);
	}
	else if (status == START_RESPOND)
	{
		_epoll.changeAbility(fd, EPOLLOUT);
	}
	else if (status == BEGIN)
	{
		_clientsArr[index].UpdateTime();
		_epoll.changeAbility(fd, EPOLLIN);
	}
	else if (status == CGI_START)
	{
		static int cgiRuns = 1;
		std::cout << "cgi runs" << cgiRuns++ << std::endl;; // debug
		_pushPipe(client.getPipeCgi(), index);
		client.initializeCGI();
	}
}

void clsFlow::_newClientProcess(int serverFd)
{
	for (size_t i = 0; i < _allServers.size(); i++)
	{
		clsServerSock &server = _allServers[i];
		sockaddr_in addr;

		int newClient = server.tryAcceptNewClient(serverFd, &addr);
		if (newClient > 0)
		{
			// i may change this get port to functoino
			sockaddr_in tempAddress;
			socklen_t temp = sizeof(tempAddress);
			int status = getsockname(serverFd, (sockaddr *)&tempAddress, &temp);

			if (HelperFunctions::changeFileToNonBlocking(newClient) == -1 || status == -1)
			{
				close(newClient);
				std::cout << "Fail to change client fd to non blocking" << std::endl;
				return;
			}

			_insertClient(newClient, addr, server.getBlock(), ntohs(tempAddress.sin_port));
			break;
		}
		else if (newClient == -1)
			break;
	}
}

void clsFlow::_pipeFlow(int fd)
{
	short index = _IdByPipe[fd];
	clsClient &client = _clientsArr[index];

	if (client.monitorCgi())
	{
		_popPipe(fd);
	}
}

void clsFlow::_flowProcess(int fd, fdTypes &TypeFd, int indexEvent)
{
	// update flow of checkins is it a pipe or a server
	if (TypeFd == CLIENT_SOCK)
		_clientProcess(fd, _clientsEvents[indexEvent].events);
	else if (TypeFd == SERVER_SOCK)
		_newClientProcess(fd);
	else if (TypeFd == PIPE)
		_pipeFlow(fd);
}

void clsFlow::_tryTimeOutClients()
{
	if (_clientIdByFd.size())
	{
		short clientFd;
		short index;
		std::map<short, short>::iterator it = _clientIdByFd.begin();
		std::map<short, short>::iterator end = _clientIdByFd.end();
		while (it != end)
		{
			clientFd = it->first;
			index = it->second;
			size_t timeConected = _clientsArr[index].GetLastConnection();
			it++;
			if (HelperFunctions::isTimeout(timeConected, TIMEOUT_CLIENT))
			{
				this->_freeClient(clientFd);
			}
		}
	}
}

void clsFlow::_tryTimeOutCgi()
{
	if (_IdByPipe.size())
	{
		std::map<short, short>::iterator it = _IdByPipe.begin();
		std::map<short, short>::iterator end = _IdByPipe.end();
		while (it != end)
		{
			int pipeFd = it->first;
			int index = it->second;
			it++;
			if (_clientsArr[index].timeoutCgi())
			{
				_popPipe(pipeFd);
			}
		}
	}
}

void clsFlow::EventLoop()
{
	int nFds = 0;
	fdTypes TypeFd;
	while (1)
	{
		while ((nFds = _epoll.tryPollNewClients(_clientsEvents, EVENTS_MAX, 1024)) > 0)
		{
			for (int i = 0; i < nFds; i++)
			{
				int fd = (_clientsEvents[i].data.fd);
				if (_clientIdByFd.size() && _clientIdByFd.count(fd))
				{
					TypeFd = CLIENT_SOCK;
				}
				else if (_IdByPipe.size() > 0 && _IdByPipe.count(fd))
					TypeFd = PIPE;
				else
					TypeFd = SERVER_SOCK;

				if (_eventsEroorHandle(_clientsEvents[i], TypeFd))
					continue;
				else
					_flowProcess(fd, TypeFd, i);
			}
			_tryTimeOutCgi();
			_tryTimeOutClients();
		}
		_tryTimeOutCgi();
		_tryTimeOutClients();
	}
}

clsFlow::~clsFlow()
{
	// free data neseccaryly
	// HelperFunctions::free_matrex(HelperFunctions::GetENV_VAR_CONST());
	delete[] _clientsArr;
}