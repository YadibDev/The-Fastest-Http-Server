#include "Client.hpp"


enum clinetState;

Client::Client(const sockaddr_in &addr, int fd) : _addr(addr),  _FirstConnection(HelperFunctions::getCurrentTimeInMs()), _socket(fd)
{
    _LastConnection = _FirstConnection;
    _state = BEGIN;
};

Client::Client(const Client &other) : _addr(other._addr),  _FirstConnection(HelperFunctions::getCurrentTimeInMs()), _socket(other._socket)
{
    _LastConnection = _FirstConnection;
    _state = BEGIN;
}

const clinetState &Client::GetState() const
{
    return _state;
}

unsigned short Client::GetPort() const
{
    return ntohs(_addr.sin_port);
}

unsigned int Client::GetIp() const
{
    return ntohl(_addr.sin_addr.s_addr);
}

size_t Client::GetTimeConnection() const
{
    return _FirstConnection;
}

size_t Client::GetLastConnection() const
{
    return _LastConnection;
}

// seters
void Client::SetState(clinetState state) // set client serve state is in request or respond or start
{
    _state = state;
}

// methods
void Client::UpdateTime() // update with the time of last reqquest
{
    _LastConnection = HelperFunctions::getCurrentTimeInMs();
}

void Client::ResetAll()
{
    this->SetState(BEGIN);
}

Client::~Client()
{
    close(_socket);
}
