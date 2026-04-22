#include "clsClient.hpp"

#define CHUNK_LIMIT 8192


clsClient::clsClient() : _dataForReq(), _RequestXconfig(_dataForReq), _Requester(_dataForReq, &_RequestXconfig) , _ResponderProecss(_RequestXconfig)
{
    this->_socket = -1;
    _dataForReq.io_chunk = _theData.io_chunk;
    _dataForReq.known_headers = _theData.known_headers;
    _dataForReq.unknown_headers = _theData.unknown_headers;
    _dataForReq.request_metadata = _theData.request_metadata;
    _dataForReq.read_body_ptr = &_theData.read_body;
    _fdRespond = 0;
    _state = BEGIN;
}

void clsClient::initializeClient(const sockaddr_in &addr, int fd, clsServerConfig *block) 
{
    _theData.Reset();
    _fdRespond = 0;
    _FirstConnection = HelperFunctions::getCurrentTimeInMs();
    _socket = fd;
    _addr = addr,
    this->block = block;
    _LastConnection = _FirstConnection;
    _state = BEGIN;
    _Requester.init(block);
    // add request
}

// clsClient::clsClient(const clsClient &other) : block(other.block),
//                                                _socket(other._socket),
//                                                _FirstConnection(other._FirstConnection),
//                                                _addr(other._addr),
//                                                _dataForReq(),
//                                                RequestXconfig(_dataForReq),
//                                                _Requester(_dataForReq, &block, &RequestXconfig),
//                                                _ResponderProecss(RequestXconfig)

// {
//     this->_dataForReq.io_chunk = this->_theData.io_chunk;
//     this->_dataForReq.known_headers = this->_theData.known_headers;
//     this->_dataForReq.unknown_headers = this->_theData.unknown_headers;
//     _dataForReq.request_metadata = _theData.request_metadata;
//     this->_dataForReq.read_body_ptr = &_theData.read_body;
//     _fdRespond = 0;
//     _LastConnection = _FirstConnection;
//     _state = BEGIN;
// }

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
    if (this->_socket > 0)
        close(this->_socket);
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
        _theData.Reset();
        _Requester.init();
        _state = REQUEST_MODE;
    }

    int size = _ReadDataForReq(); // reading data for request

    if (_state == CONNECTION_CLOSED || size == -1)
        return ;

    if (_Requester._state == RequestParser::STATE_BODY)
        _Requester.Parse(_theData.read_body);  // then pase it to parse
    else
        _Requester.Parse(_theData.read_offset - 1);  // then pase it to parse

    if (_Requester.isError())
    {
        this->_state = START_RESPOND;
        return ;
    }
    else if (_Requester.isComplete())
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

ssize_t clsClient::_addSizeChunkToStr()
{
    ssize_t byteCanSend = CHUNK_LIMIT - bytesToSend - 8 - 2;
    char *respondBuffer = this->_theData.io_chunk;

    if (byteCanSend <= 0)
        return 0;
    if (bodyLimit == 0)
    {
        memcpy(&respondBuffer[bytesToSend], "0\r\n\r\n", 5);
        _state = LAST_CHUNKED;
        bytesToSend += 5;
        return 0;
    }
    byteCanSend = min(byteCanSend, bodyLimit);

    std::string lengthHex =  HelperFunctions::Convert_Hex("0123456789abcdef", byteCanSend);
    lengthHex += "\r\n";

    memcpy(&respondBuffer[bytesToSend], lengthHex.c_str(), lengthHex.size());
    bytesToSend += lengthHex.size();
    bodyLimit -= byteCanSend;
    return byteCanSend;
}

void clsClient::_SendRespond(const clsResponse &_Responder)
{
    ssize_t s = 0;
    ssize_t nBytes;

    char *respondBuffer = this->_theData.io_chunk;

    if (_BodyPlace == bodyPlaceEnum::DISK)
    {
        if (_fdRespond == 0)
            _fdRespond = open(_Responder.GetFileName().c_str(), O_RDONLY); // error if fd == -1

        int sizeToRead = _addSizeChunkToStr();
        if (sizeToRead)
        {
            
            s = read(_fdRespond, &respondBuffer[bytesToSend], sizeToRead);
            bytesToSend += s;
            memcpy(&respondBuffer[bytesToSend], "\r\n", 2);
            bytesToSend += 2;
        }
    }

    nBytes = send(_socket, respondBuffer, bytesToSend, MSG_DONTWAIT);
    
    if (nBytes != -1)
    {
        if (nBytes < bytesToSend)
            memcpy(&respondBuffer[0], &respondBuffer[nBytes], bytesToSend - nBytes); // move data to begin        bytesToSend - nBytes == total bytes left or not send
        bytesToSend -= nBytes;
    }

    if ((bytesToSend == 0 && _BodyPlace == bodyPlaceEnum::RAM) || (_state == LAST_CHUNKED && bodyLimit <= 0))
    {
        _state = BEGIN;
        if (_Responder.GetIsConnection() == false)
            _state = CONNECTION_CLOSED;
        if (_fdRespond > 0)
        {
            close(_fdRespond);
            _fdRespond = 0;
        }
    }
}

void clsClient::_initalizeRespondBuffer(char *respondBuffer, const char *Headers, const char *Body, clsResponse &Respond)
{
    memcpy(&respondBuffer[0], Headers, bytesToSend);

    if (Respond.GetFileName().empty())
    {
        _BodyPlace = bodyPlaceEnum::RAM;
        memcpy(&respondBuffer[bytesToSend], Body, Respond.GetSizeBody());
        bytesToSend += Respond.GetSizeBody();
    }
    else
    {
        bodyLimit = Respond.GetSizeBody();
        _BodyPlace = bodyPlaceEnum::DISK;
    }
}

void clsClient::ProcessRespond()
{
    clsResponse &Respond = _ResponderProecss.GetclsResponse();

    char *respondBuffer = this->_theData.io_chunk;

    if (_state == START_RESPOND)
    {
        bytesToSend = 0;
        _state = RESPOND_MODE;

        // linke request with config
        Respond.Reset();
        this->_ResponderProecss.MainProcess(); // create respond

        bytesToSend = Respond.GetHeaderFeild().size();

        const char *Header;
        const char *Body;
        if (Respond.GetModTransferData())
        {
            Header = Respond.GetHeaderFeildPointer()->c_str();
            Body = Respond.GetBodyPointer()->c_str();
        }
        else
        {
            Header = Respond.GetHeaderFeild().c_str();
            Body = Respond.GetBody().c_str();
        }
        _initalizeRespondBuffer(respondBuffer, Header, Body, Respond);
    }
    _SendRespond(Respond);
}

void clsClient::ProcessBoth(uint32_t events)
{
    if ((events & EPOLLIN) == EPOLLIN)
        ProcessRequest();
    else if ((events & EPOLLOUT) == EPOLLOUT)
        ProcessRespond();
}

void clsClient::freeRessources()
{
    if (this->_socket > 0)
        close(this->_socket);
    _socket = -1;
}
