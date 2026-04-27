#include "clsClient.hpp"

#define CHUNK_LIMIT 8192

clsClient::clsClient() : _dataForReq(), _RequestXconfig(_dataForReq), _Requester(_dataForReq, &_RequestXconfig), _ResponderProecss(_RequestXconfig)
{
    // _Requester.init();
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
    _RequestXconfig.reset();
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
    _theData.Reset();
    _Requester.init();
    _ResponderProecss.Reset();
    _monitorCGI.freeCgiRessources();
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
   
    HttpError error;
    if (_state == BEGIN)
    {
        ResetAll();
        _state = REQUEST_MODE;
    }

    int size = _ReadDataForReq(); // reading data for request

    if (_state == CONNECTION_CLOSED || size == -1)
        return;

    if (_Requester._state == RequestParser::STATE_BODY)
        _Requester.Parse(_theData.read_body); // then pase it to parse
    else
        _Requester.Parse(_theData.read_offset - 1); // then pase it to parse

    if (_Requester.isError())
    {
        if (ProcessRequestHandler::generateErrorPath(_Requester.getError().getCodeStatus(), this->block, &_RequestXconfig, error))
        {
            _RequestXconfig.setDefaultErrorPage(true);
        }
        this->_state = START_RESPOND;
        return;
    }
    else if (_Requester.isComplete())
    {
        this->_state = START_RESPOND;
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

    std::string lengthHex = HelperFunctions::Convert_Hex("0123456789abcdef", byteCanSend);
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
        {
            if (_Responder.GetModTransferData() == false)
                _fdRespond = open(_Responder.GetFileName().c_str(), O_RDONLY); // error if fd == -1
            else
                _fdRespond = open(_Responder.GetFileFromDiskPointer()->c_str(), O_RDONLY); // error if fd == -1
        }

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

void clsClient::_initalizeRespondBuffer()
{
    clsResponse &Respond = _ResponderProecss.GetclsResponse();
    const char *Header;
    const char *Body;
    bool fileExist = false;
    char *respondBuffer = this->_theData.io_chunk;
    if (Respond.GetModTransferData())
    {
        bytesToSend += Respond.GetHeaderFeildPointer()->size();
        Header = Respond.GetHeaderFeildPointer()->c_str();
        Body = Respond.GetBodyPointer()->c_str();
        fileExist = Respond.GetFileFromDiskPointer()->size() > 0; // check is greater than 0
    }
    else
    {
        bytesToSend += Respond.GetHeaderFeild().size();
        Header = Respond.GetHeaderFeild().c_str();
        Body = Respond.GetBody().c_str();
        fileExist = Respond.GetFileName().size() > 0; // check is greater than 0
    }

    memcpy(&respondBuffer[0], Header, bytesToSend);

    if (fileExist == false)
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

int clsClient::getPipeCgi()
{
    return _ResponderProecss.GetclsCGI().GetFdPipe();
}

void clsClient::ProcessRespond()
{
    clsResponse &Respond = _ResponderProecss.GetclsResponse();

    if (_state == START_RESPOND)
    {
        bytesToSend = 0;
        _state = RESPOND_MODE;

        this->_ResponderProecss.MainProcess(); // create respond

        if (this->_ResponderProecss.isRunCgi())
        {
            _state = CGI_START;
            return;
        }
        _initalizeRespondBuffer();
    }
    else if (_state == CGI_END)
    {
        _initalizeRespondBuffer();
        _state = RESPOND_MODE;
    }

    if (_state == RESPOND_MODE)
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
    ResetAll();
    // _Requester.init();
    // _monitorCGI.freeCgiRessources();
    // _ResponderProecss.GetclsResponse().Reset();

    
    std::cout << _socket << std::endl;
    if (this->_socket > 0)
        close(this->_socket);
    _socket = -1;
}

void clsClient::logs()
{
    string arr[3] = {"GET", "POST", "DELETE"};
    std::cout << "\n================= log start =================" << std::endl;
    const RequestLine &reqLine = _Requester.getRequestLine();
    std::cout << arr[(int)reqLine.getMethod()] << " ";
    for (int i = 0; i < reqLine.getRequestURI().getPath().len; i++)
        std::cout << reqLine.getRequestURI().getPath().Data[i];
    std::cout << " ";
    for (int i = 0; i < reqLine.getVersion().len; i++)
        std::cout << reqLine.getVersion().Data[i];
    for (int i = 0; i < 6; i++)
    {
        if (this->_theData.known_headers[i].Hash != -1)
        {
            cout << std::endl;
            for (int i = 0; i < this->_theData.known_headers[i].key.len; i++)
                std::cout << this->_theData.known_headers[i].key.Data[i];
            std::cout << ": ";
            for (int i = 0; i < this->_theData.known_headers[i].val.len; i++)
                std::cout << this->_theData.known_headers[i].val.Data[i];
        }
    }
    std::cout << "\nPhysical path: " << _RequestXconfig.getPhysicalPath() << std::endl;
    std::cout << "Status code from request : " << _Requester.getError().getCodeStatus() << endl;
    std::cout << "\n================= log end =================\n"
              << std::endl;
}

void clsClient::initializeCGI()
{
    clsCGI &cgi = this->_ResponderProecss.GetclsCGI();
    _state = CGI_RUNING;
    _monitorCGI.initialzie(cgi.GetPid(), cgi.GetFdPipe(), cgi.getStartTime());
}

bool clsClient::monitorCgi()
{
    static int i = 0;
    std::cout << "================\n";
    std::cout << i++ << endl;
    std::cout << "================\n";

    short length = _monitorCGI.getDataFromCgi(_theData.io_chunk, sizeof(_theData.io_chunk));
    stEventProcess::eEventProcess processState = _monitorCGI.getStateProcess();
    stEventData::eEventData dataState = _monitorCGI.getStateData();

    if (length == -1)
    {
        _state = CGI_END;
        _ResponderProecss.setEventProcess(processState);
        _ResponderProecss.ParseCGI(NULL, 0);
        return true;
    }

    else if (length == 0 || (processState == stEventProcess::THE_END && dataState == stEventData::END_PIPE))
    {
        _state = CGI_END;
        _ResponderProecss.setEventProcess(processState);
        _ResponderProecss.ParseCGI(_theData.io_chunk, length);
        return true;
    }

    _ResponderProecss.ParseCGI(_theData.io_chunk, length);

    if (_ResponderProecss.getEventProcess() == stEventProcess::END_WITH_PARSE)
    {
        _monitorCGI.freeCgiRessources();
        return true;
    }
    return false;
}

bool clsClient::timeoutCgi()
{
    if (_monitorCGI.getStateData() == stEventData::END_PIPE && _monitorCGI.getStateProcess() != stEventProcess::RUNINNG)
        return true;
    if (_monitorCGI.TimeoutCgi())
    {
        _state = CGI_END;
        _ResponderProecss.setEventProcess(stEventProcess::END_WITH_TIMOUT);
        _ResponderProecss.ParseCGI(NULL, 0);
        return true;
    }
    return false;
}
