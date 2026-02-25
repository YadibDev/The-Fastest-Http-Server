#include "clsLinker.hpp"

clsLinker::clsLinker()
{
}

clsLinker::~clsLinker()
{
    map<int, clsClient>::iterator it = _clientsDB.begin();
    map<int, clsClient>::iterator end = _clientsDB.end();

    while (it != end)
    {
        close((*it).first);
        ++it;
    }

    this->_clientsDB.clear();
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "clsLinker closed all clients fd" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
}

clsClient &clsLinker::GetClientAt(int fd)
{
    return this->_clientsDB.at(fd);
}

void clsLinker::removeClient(int fd)
{
    this->_clientsDB.erase(fd);
    close(fd);
}

void clsLinker::insertClient(int fd, const sockaddr_in &addr)
{
    _clientsDB.insert(std::pair<int, clsClient>(fd, clsClient(addr, fd)));
}

// i will create it 
// void clsLinker::TimeOutClients()
// {
//     unsigned long long totalMs;
//     map<int, clsClient>::iterator it = this->_clientsDB.begin();
//     map<int, clsClient>::iterator end = this->_clientsDB.end();

//     totalMs = HelperFunctions::getCurrentTimeInMs();

//     while (it != end)
//     {
//         // check here diffrent between two
//         // it.operator->
//         ++it;
//     }
// };