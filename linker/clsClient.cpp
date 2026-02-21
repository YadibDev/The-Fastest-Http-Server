#include "clsClient.hpp"

#define CHUNK_LIMIT 4096
clsClient::clsClient(const sockaddr_in &addr, int fd) : _addr(addr), _FirstConnection(HelperFunctions::getCurrentTimeInMs()), _socket(fd)
{
    _LastConnection = _FirstConnection;
    _state = BEGIN;
};

clsClient::clsClient(const clsClient &other) : _addr(other._addr), _FirstConnection(HelperFunctions::getCurrentTimeInMs()), _socket(other._socket)
{
    _LastConnection = _FirstConnection;
    _state = BEGIN;
}

const clinetState &clsClient::GetState() const
{
    return _state;
}

unsigned short clsClient::GetPort() const
{
    return ntohs(_addr.sin_port);
}

unsigned int clsClient::GetIp() const
{
    return ntohl(_addr.sin_addr.s_addr);
}

size_t clsClient::GetTimeConnection() const
{
    return _FirstConnection;
}

size_t clsClient::GetLastConnection() const
{
    return _LastConnection;
}

// seters
void clsClient::SetState(clinetState state) // set client serve state is in request or respond or start
{
    _state = state;
}

// methods
void clsClient::UpdateTime() // update with the time of last reqquest
{
    _LastConnection = HelperFunctions::getCurrentTimeInMs();
}

void clsClient::ResetAll()
{
    this->SetState(BEGIN);
    // i will add reset of request and respond
}

clsClient::~clsClient()
{
}


void clsClient::ProcessRequest()
{

}


// i should create the logic of this and improve it
void clsClient::_SendRespond(clsResponse &_Responder)
{
    string respond;
    ssize_t s;
    ssize_t s_send;

    if (_DataLeft.empty() == false)
    {
        s_send = send(_socket, _DataLeft.c_str(), _DataLeft.size(), MSG_DONTWAIT);
        // if s_send == -1 is it possible ? what should i do
        _DataLeft = &_DataLeft[s_send];
    }
    else if (_BodyPlace == RAM)
    {
        _state = SEND_BODY;
        respond = _Responder.GetBody();
        s_send = send(_socket, respond.c_str(), respond.size(), MSG_DONTWAIT);
        _DataLeft = &respond[s_send];
    }
    else if (_BodyPlace == DISK_FILE)
    {
        _state = SEND_BODY;
        respond.resize(CHUNK_LIMIT);
        if (_fdRespond == 0)
        {
            _fdRespond = open(_Responder.GetFileName().c_str(), O_RDONLY);
            // if (_fdRespond == -1)
            //     ; // we should handle this edge case
        }

        s = read(_fdRespond, &respond[0], CHUNK_LIMIT);
        if (s < CHUNK_LIMIT)
        {
            if (s == 0)
            {
                _state = LAST_CHUNKED;
                close(_fdRespond);
            }
        }
        respond.resize(s);
        respond = _Responder.ChunkData(respond);
        s_send = send(_socket, respond.c_str(), respond.size(), MSG_DONTWAIT);
        _DataLeft = &respond[s_send];
    }

    if (_DataLeft.empty() && ((_BodyPlace == RAM && _state == SEND_BODY) || _state == LAST_CHUNKED))
        _state = BEGIN;
}

void clsClient::ProcessRespond()
{
    if (_state == REQUEST_MODE)
    {
        // initialized the reponder
        _DataLeft = "";
        string Header;
        ssize_t s;
        string fileName = "index.html";

        _Responder.SetFileFromDisk(fileName);
        _Responder.SetMod(GET);
        _Responder.SetStatus(200);
        _Responder.SetType("text/html");

        
         _Responder.MakeResponse();
         // start from here
        if (_Responder.GetFileName().empty())
            _BodyPlace = RAM;
        else
            _BodyPlace = DISK_FILE;
        _state = RESPOND_MODE;
        _fdRespond = 0;

        s = send(_socket, &Header[0], Header.size(), MSG_DONTWAIT);
        if (s >= 0 && s < Header.size())
            this->_DataLeft = &Header[s];
        // if s == -1 is it possible ?/
    }
    _SendRespond(_ResponderProecss.GetclsResponse()); //
}