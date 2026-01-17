#ifndef ___LINKER_HPP___
#define ___LINKER_HPP___

#include "server/EpollHandler.hpp"
#include <map>

struct Client{};

class Linker
{
private:
    EpollHandler _EventHanler;
    map<int, Client> _clientsDB;

public:
	Linker(ServerSock &SocketsServer);
    ~Linker();

    Client &GetClientAt(int fd);
    void TimeOutClients();
    void removeClient(int fd);
    void insertClient(int fd);
};

#endif