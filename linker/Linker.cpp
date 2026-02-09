#include "Linker.hpp"

Linker::Linker(ServerSock &SocketsServer)
{

}

Linker::~Linker()
{
    map<int, Client>::iterator it = _clientsDB.begin();
    map<int, Client>::iterator end = _clientsDB.end();

    while (it != end)
    {
        close((*it).first);
        ++it;
    }

    this->_clientsDB.clear();
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "Linker closed all clients fd" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
}

Client &Linker::GetClientAt(int fd)
{
    return this->_clientsDB.at(fd);
}

void Linker::removeClient(int fd)
{
    this->_clientsDB.erase(fd);
    close(fd);
}

void Linker::insertClient(int fd, const sockaddr_in &addr)
{
    this->_clientsDB[fd] = Client(addr);
}

void TimeOutClients()
{
    // add algo in future
};