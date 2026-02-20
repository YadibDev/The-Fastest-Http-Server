#include "Monitor.hpp"

Monitor::Monitor()
{

}

Monitor::~Monitor()
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
    std::cout << "Monitor closed all clients fd" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
}



Client &Monitor::GetClientAt(int fd)
{
    return this->_clientsDB.at(fd);
}

void Monitor::removeClient(int fd)
{
    this->_clientsDB.erase(fd);
    close(fd);
}

void Monitor::insertClient(int fd, const sockaddr_in &addr)
{
    _clientsDB.insert( std::pair<int, Client>(fd, Client(addr, fd)) );
}

void Monitor::TimeOutClients()
{
    unsigned long long totalMs;
    map<int, Client>::iterator it = this->_clientsDB.begin();
    map<int, Client>::iterator end = this->_clientsDB.end();

    totalMs = HelperFunctions::getCurrentTimeInMs(); 

    while (it != end)
    {
        // check here diffrent between two
        // it.operator->
        ++it;
    }

};