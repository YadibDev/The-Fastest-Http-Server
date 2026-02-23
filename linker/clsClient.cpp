#include "clsClient.hpp"

#define CHUNK_LIMIT 1024 * 500
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
    string buffer;

    buffer.resize(4096);
    int size = recv(this->_socket, &buffer[0], 4096, MSG_DONTWAIT);
    if (size == 0)
    {
        _state = CONNECTION_CLOSED;
        return ;
    }
    buffer.resize(size);
    if (_state == BEGIN) _Requester._Buffer = "", _state = REQUEST_MODE;
        // i must reset the requester
    
    _Requester.parse(buffer);

    if (_Requester.isCompleted()) // is in error case also like this    
    {
        this->_state = START_RESPOND;
        return ;
    }
}

// i should create the logic of this and improve it
void clsClient::_SendRespond(const clsResponse &_Responder)
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
    else if (_BodyPlace == DISK_FILE)
    {
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

    if (_DataLeft.empty() && (_BodyPlace == RAM || _state == LAST_CHUNKED))
    {
        _state = BEGIN;
        if (_Responder.GetIsConnection() == false)
            _state = CONNECTION_CLOSED;
    }
}

void clsClient::ProcessRespond(const clsServerConfig &serverConfig)
{
    if (_state == START_RESPOND)
    {
        // initialized the reponder
        _state = RESPOND_MODE;
        _fdRespond = 0;
        _DataLeft = "";
        ssize_t s;
        const clsResponse & Respond = _ResponderProecss.GetclsResponse();

        // linke request with config
        ProcessRequestHandler::processRequest(this->_Requester, serverConfig, RequestXconfig);

        this->_ResponderProecss.MainProcess(RequestXconfig); // create respond

        const string &Header = Respond.GetHeaderFeild();
        if (Respond.GetFileName().empty())
        {
            _DataLeft = Respond.GetBody();
            _BodyPlace = RAM;
        }
        else
            _BodyPlace = DISK_FILE;

        _DataLeft = Header + _DataLeft;
        s = send(_socket, &_DataLeft[0], _DataLeft.size(), MSG_DONTWAIT); // can return -1
        this->_DataLeft = &_DataLeft[s];
    }
    _SendRespond(_ResponderProecss.GetclsResponse()); //
}