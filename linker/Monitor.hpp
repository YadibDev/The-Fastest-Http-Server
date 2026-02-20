#ifndef ___Monitor_HPP___
#define ___Monitor_HPP___

#include "../server/EpollHandler.hpp"
#include "Client.hpp"
#include <map>
#include <sys/time.h>
#include "../Utils/HelperFunctions.hpp"


class Monitor
{
private:
    map<int, Client> _clientsDB;

public:
	Monitor();
    ~Monitor();

    Client &GetClientAt(int fd);
    void TimeOutClients();
    void removeClient(int fd);
    void insertClient(int fd, const sockaddr_in &addr);
};

#endif