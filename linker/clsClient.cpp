#include "clsClient.hpp"

#define CHUNK_LIMIT 2 * 1024 * 1024

clsClient::clsClient(const sockaddr_in &addr, int fd, clsServerConfig &block) : block(block),
                                                                                _socket(fd),                                             // must be not const
                                                                                _FirstConnection(HelperFunctions::getCurrentTimeInMs()), // must be not const
                                                                                _addr(addr),
                                                                                _dataForReq(),
                                                                                RequestXconfig(_dataForReq),
                                                                                _Requester(_dataForReq, &block, &RequestXconfig),
                                                                                _ResponderProecss(RequestXconfig)
{
    this->_dataForReq.io_chunk = this->_theData.io_chunk;
    this->_dataForReq.known_headers = this->_theData.known_headers;
    this->_dataForReq.unknown_headers = this->_theData.unknown_headers;
    _dataForReq.request_metadata = _theData.request_metadata;
    this->_dataForReq.sizeUnknownHeaders = 25; // unknown_headers[25];
    this->_dataForReq.read_body_ptr = &_theData.read_body;
    _fdRespond = 0;
    _LastConnection = _FirstConnection;
    _state = BEGIN;
};

clsClient::clsClient(const clsClient &other) : block(other.block),
                                               _socket(other._socket),
                                               _FirstConnection(other._FirstConnection),
                                               _addr(other._addr),
                                               _dataForReq(),
                                               RequestXconfig(_dataForReq),
                                               _Requester(_dataForReq, &block, &RequestXconfig),
                                               _ResponderProecss(RequestXconfig)

{
    this->_dataForReq.io_chunk = this->_theData.io_chunk;
    this->_dataForReq.known_headers = this->_theData.known_headers;
    this->_dataForReq.unknown_headers = this->_theData.unknown_headers;
    _dataForReq.request_metadata = _theData.request_metadata;
    this->_dataForReq.sizeUnknownHeaders = 25;
    this->_dataForReq.read_body_ptr = &_theData.read_body;
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
    // if (_fdRespond > 0)
    //     close(_fdRespond);
}

int clsClient::_ReadDataForReq()
{
    int size = 0;

    if (_Requester._state == RequestParser::STATE_REQUEST_LINE || _Requester._state == RequestParser::STATE_HEADERS)
    {
        uint16_t &idx = _theData.read_offset;
        size = recv(_socket, &_theData.request_metadata[idx], (16384 - idx), MSG_DONTWAIT);
        if (size > 0)
            idx += size;
    }
    else if (_Requester._state == RequestParser::STATE_BODY)
    {
        // add edge case if data still in request meta data
        uint16_t &idx = _theData.read_body;
        size = recv(_socket, &_theData.io_chunk[idx], (8192 - idx), MSG_DONTWAIT);
        if (size > 0)
            idx += size;
    }

    if (size == 0)
        _state = CONNECTION_CLOSED;

    return size;
}

void clsClient::ProcessRequest()
{
    // reset request in every new request from client
    if (_state == BEGIN)
    {
        _state = REQUEST_MODE;
        _theData.Reset();
    }

    int size = _ReadDataForReq(); // reading data for request

    if (_state == CONNECTION_CLOSED || size == -1)
        return ;

    if (_Requester._state == RequestParser::STATE_BODY)
        _Requester.Parse(_theData.read_body);  // then pase it to parse
    else
        _Requester.Parse(_theData.read_offset - 1);  // then pase it to parse

    if (_Requester.isError())
        std::cout << "Error while reading " << std::endl;

    if (_Requester.isComplete()) // add get error here
    {
        this->_state = START_RESPOND;

        std::cout << "\n================= REQUEST DEBUG =================\n";

        std::cout << "[STATUS] Request completed successfully\n";

        std::cout << "\n[METADATA]\n";
        std::cout << this->_theData.request_metadata << "\n";

        std::cout << "\n[BODY STORAGE]\n";
        if (_Requester._body._bodyLocation == clsBody::DISK)
            std::cout << "Location : DISK\n";
        else if (_Requester._body._bodyLocation == clsBody::RAM)
            std::cout << "Location : RAM\n";
        else
            std::cout << "Location : UNKNOWN (" << _Requester._body._bodyLocation << ")\n";

        std::cout << "\n[BODY INFO]\n";
        if (this->_theData.read_body > 0)
        {
            std::cout << "Size : " << this->_theData.read_body << " bytes\n";
            std::cout << "Content:\n";
            std::cout << "----------------------------------------\n";

            for (int i = 0; i < _theData.read_body; i++)
                std::cout << this->_theData.io_chunk[i];

            std::cout << "\n----------------------------------------\n";
        }
        else
        {
            std::cout << "No body received\n";
        }

        std::cout << "========================================\n\n";
        return;
    }
}

void clsClient::_SendRespond(const clsResponse &_Responder)
{
    ssize_t s;
    ssize_t nBytes;

    if (_BodyPlace == bodyPlaceEnum::DISK)
    {
        string chunkData;

        // resize single time
        chunkData.resize(CHUNK_LIMIT);
        if (_fdRespond == 0)
            _fdRespond = open(_Responder.GetFileName().c_str(), O_RDONLY);
        s = read(_fdRespond, &chunkData[0], CHUNK_LIMIT);
        // i woill work here for
        if (s < CHUNK_LIMIT)
        {
            _state = LAST_CHUNKED;
            chunkData.resize(s);
            // if last respond we will add the end
            _Responder.ChunkData(respondBuffer, chunkData, true);
        }
        else
        {
            chunkData.resize(s);
            _Responder.ChunkData(respondBuffer, chunkData, false);
        }
    }

    // start sending data
    nBytes = send(_socket, respondBuffer.c_str(), respondBuffer.size(), MSG_DONTWAIT);
    if (nBytes != -1)
        respondBuffer = &respondBuffer[nBytes];

    if (respondBuffer.empty() && (_BodyPlace == bodyPlaceEnum::RAM || _state == LAST_CHUNKED))
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

void clsClient::ProcessRespond()
{
    clsResponse &Respond = _ResponderProecss.GetclsResponse();
    if (_state == START_RESPOND)
    {
        _state = RESPOND_MODE;

        // linke request with config

        this->_ResponderProecss.MainProcess(); // create respond

        respondBuffer += Respond.GetHeaderFeild();

        std::cout << "--------- size: " << respondBuffer.size() << endl;;
        std::cout << respondBuffer;
        std::cout << endl;
        if (Respond.GetFileName().empty())
        {
            respondBuffer += Respond.GetBody();
            _BodyPlace = bodyPlaceEnum::RAM;
        }
        else
            _BodyPlace = bodyPlaceEnum::DISK;
    }
    _SendRespond(Respond);
}