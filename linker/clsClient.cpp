#include "clsClient.hpp"

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
    _FirstConnection = HelperFunctions::getCurrentTimeInS();
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

long clsClient::GetTimeConnection() const
{
    return _FirstConnection;
}

long clsClient::GetLastConnection() const
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
    _LastConnection = HelperFunctions::getCurrentTimeInS();
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
        size = recv(_socket, &_theData.request_metadata[idx], (SIZE_BUFFER - idx), 0);
        if (size > 0)
            idx += size;

        if (size == 0 && (SIZE_BUFFER - idx) > 0)
            _state = CONNECTION_CLOSED;
    }
    else if (_Requester._state == RequestParser::STATE_BODY)
    {
        uint16_t &idx = _theData.read_body;
        size = recv(_socket, &_theData.io_chunk[idx], (SIZE_BUFFER - idx), 0);
        if (size > 0)
            idx += size;

        if (size == 0 && (SIZE_BUFFER - idx) > 0)
            _state = CONNECTION_CLOSED;
    }

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

    int size = _ReadDataForReq();

    if (_state == CONNECTION_CLOSED || size == -1 || size == 0)
        return;

    if (_Requester._state == RequestParser::STATE_BODY)
        _Requester.Parse(_theData.read_body);
    else
        _Requester.Parse(_theData.read_offset - 1);

    if (_Requester.isError())
    {
        if (!ProcessRequestHandler::generateErrorPath(_Requester.getError().getCodeStatus(), this->block, &_RequestXconfig, error))
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

short clsClient::_addSizeChunkToStr()
{
    short byteCanSend = SIZE_BUFFER - bytesToSend - 8 - 2;
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
    byteCanSend = min<ssize_t>(byteCanSend, bodyLimit);

    std::string lengthHex = HelperFunctions::Convert_Hex("0123456789abcdef", byteCanSend);
    lengthHex += "\r\n";

    memcpy(&respondBuffer[bytesToSend], lengthHex.c_str(), lengthHex.size());
    bytesToSend += lengthHex.size();
    bodyLimit -= byteCanSend;
    return byteCanSend;
}

void clsClient::_LoadAutoIndex(clsResponse &_Responder)
{
    char *respondBuffer = this->_theData.io_chunk;

    bool isFinished = _Responder.fetchAutoIndex(respondBuffer, bytesToSend, SIZE_BUFFER - bytesToSend);
    if (isFinished)
        _state = AUTO_INDEX_DONE;
}

void clsClient::_SendRespond(clsResponse &_Responder)
{
    ssize_t s = 0;
    ssize_t nBytes;

    char *respondBuffer = this->_theData.io_chunk;

    if (_BodyPlace == bodyPlaceEnum::DISK)
    {
        if (_fdRespond == 0)
        {
            if (_Responder.GetModTransferData() == false)
                _fdRespond = open(_Responder.GetFileName().c_str(), (O_RDONLY | O_CLOEXEC)); // error if fd == -1
            else
                _fdRespond = open(_Responder.GetFileFromDiskPointer()->c_str(), O_RDONLY | O_CLOEXEC); // error if fd == -1
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
    else if (_BodyPlace == bodyPlaceEnum::AUTO_INDEX)
    {
        _LoadAutoIndex(_Responder);
    }

    nBytes = send(_socket, respondBuffer, bytesToSend, 0);

    if (nBytes != -1)
    {
        if (nBytes < bytesToSend)
            memcpy(&respondBuffer[0], &respondBuffer[nBytes], bytesToSend - nBytes); // move data to begin        bytesToSend - nBytes == total bytes left or not send
        bytesToSend -= nBytes;
    }

    if ((bytesToSend == 0 && _BodyPlace == bodyPlaceEnum::RAM) || (_state == LAST_CHUNKED && bodyLimit <= 0 && bytesToSend == 0) || _state == AUTO_INDEX_DONE)
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

void clsClient::_handleInternal()
{
    clsResponse &Respond = _ResponderProecss.GetclsResponse();
    // support internal location in future
    if (Respond.IsError())
    {
        std::cout << "is respond Errror \n"
                  << std::endl;
        ;
        HttpError error;
        if (!ProcessRequestHandler::generateErrorPath(Respond.GetStatus(), this->block, &_RequestXconfig, error))
        {
            _RequestXconfig.setDefaultErrorPage(true);
        }
        _ResponderProecss.Reset();
        this->_ResponderProecss.MainProcess(); // re create error page
    }
}

void clsClient::_initalizeRespondBuffer()
{
    clsResponse &Respond = _ResponderProecss.GetclsResponse();
    const char *Header;
    const char *Body;
    bool fileExist = false;
    char *respondBuffer = this->_theData.io_chunk;

    _handleInternal(); // beta;
    if (Respond.GetModTransferData())
    {
        bytesToSend += Respond.GetHeaderFeildPointer()->size();
        Header = Respond.GetHeaderFeildPointer()->c_str();
        Body = Respond.GetBodyPointer()->c_str();
        fileExist = Respond.GetFileFromDiskPointer()->size() > 0; // check is greater than 0
    }
    else if (Respond.IsAutoIndex())
    {
        bytesToSend += Respond.GetHeaderFeild().size();
        Header = Respond.GetHeaderFeild().c_str();
    }
    else
    {
        bytesToSend += Respond.GetHeaderFeild().size();
        Header = Respond.GetHeaderFeild().c_str();
        Body = Respond.GetBody().c_str();
        fileExist = Respond.GetFileName().size() > 0; // check is greater than 0
    }

    memcpy(&respondBuffer[0], Header, bytesToSend);

    if (Respond.IsAutoIndex())
    {
        _BodyPlace = bodyPlaceEnum::AUTO_INDEX;
    }
    else if (fileExist == false)
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
        std::cout << "cgi end" << std::endl;
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
    if (this->_socket > 0)
    {
        close(this->_socket);
    }
    _socket = -1;
}

void clsClient::logs()
{
    static std::ofstream logFile("./logs/logfile.txt");

    if (logFile.is_open())
    {
        string arr[3] = {"GET", "POST", "DELETE"};
        logFile << "\n================= log start client =================" << endl;

        _theData.request_metadata[_theData.read_offset] = '\0';
        logFile << this->_theData.request_metadata << std::endl;

        const RequestLine &reqLine = _Requester.getRequestLine();
        logFile << arr[(int)reqLine.getMethod()] << " ";
        for (int i = 0; i < reqLine.getRequestURI().getPath().len; i++)
            logFile << reqLine.getRequestURI().getPath().Data[i];
        logFile << " ";
        for (int i = 0; i < reqLine.getVersion().len; i++)
            logFile << reqLine.getVersion().Data[i];

        logFile << "\nPhysical path: " << _RequestXconfig.getPhysicalPath() << endl;
        logFile << "Status code from request : " << _Requester.getError().getCodeStatus() << endl;
        logFile << "\n================= log end =================\n"
                << endl;
    }
}

void clsClient::initializeCGI()
{
    clsCGI &cgi = this->_ResponderProecss.GetclsCGI();
    _state = CGI_RUNING;
    _monitorCGI.initialzie(cgi.GetPid(), cgi.GetFdPipe(), cgi.getStartTime());
}

bool clsClient::monitorCgi()
{

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

    else if (processState == stEventProcess::THE_END && dataState == stEventData::END_PIPE)
    {
        _state = CGI_END;
        _ResponderProecss.setEventProcess(processState);
        _ResponderProecss.ParseCGI(_theData.io_chunk, length);
        return true;
    }

    _ResponderProecss.ParseCGI(_theData.io_chunk, length);

    if (_ResponderProecss.getEventProcess() == stEventProcess::END_WITH_PARSE)
    {
        _state = CGI_END;
        _monitorCGI.freeCgiRessources();
        return true;
    }
    return false;
}

bool clsClient::timeoutCgi()
{
    if (_monitorCGI.getStateData() == stEventData::END_PIPE && _monitorCGI.getStateProcess() != stEventProcess::RUNINNG)
        return true;
    if (_monitorCGI.TimeoutCgi()) // update also the process
    {
        _state = CGI_END;
        _ResponderProecss.setEventProcess(stEventProcess::END_WITH_TIMOUT);
        _ResponderProecss.ParseCGI(NULL, 0);
        return true;
    }
    else if (_monitorCGI.getStateData() == stEventData::END_PIPE && _monitorCGI.getStateProcess() != stEventProcess::RUNINNG)
    {
        this->_state = CGI_END;
        _ResponderProecss.setEventProcess(_monitorCGI.getStateProcess());
        _ResponderProecss.ParseCGI(NULL, 0);
        return true;
    }
    return false;
}
