#ifndef ___Monitor_HPP___
#define ___Monitor_HPP___

#include "../server/clsEpollHandler.hpp"
#include "clsClient.hpp"
#include <map>
#include <sys/time.h>
#include "../Utils/HelperFunctions.hpp"

class clsLinker
{
private:
    map<int, clsClient> _clientsDB;

public:
    clsLinker();
    ~clsLinker();

    clsClient &GetClientAt(int fd);
    void TimeOutClients();
    void removeClient(int fd);
    void insertClient(int fd, const sockaddr_in &addr, clsServerConfig &block);
};

#endif