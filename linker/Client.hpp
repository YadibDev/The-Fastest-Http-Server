#ifndef ___CLIENT_HPP___
#define ___CLIENT_HPP___

#include <iostream>
#include <netinet/in.h>

class Client
{
private:

public:
    Client(const sockaddr_in &addr);

};

#endif