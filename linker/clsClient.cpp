#include "clsClient.hpp"

#define CHUNK_LIMIT 2 * 1024 * 1024
clsClient::clsClient(const sockaddr_in &addr, int fd) : _addr(addr), _FirstConnection(HelperFunctions::getCurrentTimeInMs()), _socket(fd)
{
    _fdRespond = 0;
    _LastConnection = _FirstConnection;
    _state = BEGIN;
};

clsClient::clsClient(const clsClient &other) : _addr(other._addr), _FirstConnection(HelperFunctions::getCurrentTimeInMs()), _socket(other._socket)
{
    _fdRespond = 0;
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
    if (_fdRespond > 0)
        close(_fdRespond);
}

void clsClient::ProcessRequest()
{
    string buffer;

    // when we will add post i may modied the logic
    buffer.resize(4096);
    int size = recv(this->_socket, &buffer[0], 4096, MSG_DONTWAIT);
    if (size == 0)
    {
        _state = CONNECTION_CLOSED;
        return;
    }
    if (size == -1)
        return;
    buffer.resize(size);

    // reset request in every new request from client
    if (_state == BEGIN)
        _Requester._Buffer = "", _Requester._state = READING_LINE, _state = REQUEST_MODE;

    _Requester.parse(buffer);
    if (_Requester.isCompleted()) // add get error here
    {
        this->_state = START_RESPOND;
        return;
    }
}

void clsClient::_SendRespond(const clsResponse &_Responder)
{
    ssize_t s;
    ssize_t nBytes;

    if (_BodyPlace == DISK_FILE)
    {
        string chunkData;

        chunkData.resize(CHUNK_LIMIT);
        if (_fdRespond == 0)
            _fdRespond = open(_Responder.GetFileName().c_str(), O_RDONLY);
        s = read(_fdRespond, &chunkData[0], CHUNK_LIMIT);
        if (s < CHUNK_LIMIT)
        {
            _state = LAST_CHUNKED;
            chunkData.resize(s);
            respondBuffer += _Responder.ChunkData(chunkData);
            // if last respond we will add the end
            if (s != 0)
                respondBuffer += _Responder.ChunkData(""); 
        }
        else
        {
            chunkData.resize(s);
            respondBuffer += _Responder.ChunkData(chunkData);
        }
    }

    // start sending data
    nBytes = send(_socket, respondBuffer.c_str(), respondBuffer.size(), MSG_DONTWAIT);
    if (nBytes != -1)
        respondBuffer = &respondBuffer[nBytes];

    if (respondBuffer.empty() && (_BodyPlace == RAM || _state == LAST_CHUNKED))
    {
        _state = BEGIN;
        respondBuffer = "";
        if (_Responder.GetIsConnection() == false)
            _state = CONNECTION_CLOSED;
        if (_fdRespond > 0)
        {
            close(_fdRespond);
            _fdRespond = 0;
        }
        
    }
}

void clsClient::ProcessRespond(const clsServerConfig &serverConfig)
{
    clsResponse &Respond = _ResponderProecss.GetclsResponse();
    if (_state == START_RESPOND)
    {
        _state = RESPOND_MODE;

        // linke request with config
        ProcessRequestHandler::processRequest(this->_Requester, serverConfig, RequestXconfig);

        this->_ResponderProecss.MainProcess(RequestXconfig); // create respond

        respondBuffer += Respond.GetHeaderFeild();

        if (Respond.GetFileName().empty())
        {
            respondBuffer += Respond.GetBody();
            _BodyPlace = RAM;
        }
        else
            _BodyPlace = DISK_FILE;
    }
    _SendRespond(Respond);
}