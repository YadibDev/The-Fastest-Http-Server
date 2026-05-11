#ifndef ___CLIENT_HPP___
#define ___CLIENT_HPP___

#include <iostream>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include "../Utils/HelperFunctions.hpp"
#include "../PartRespond/mainprocess/Webserv.hpp"
#include "../Parser/RequestHandler/RequestHandler.hpp"
#include "../Parser/RequestHandler/ProcessRequestHandler.hpp"
#include "../Parser/ParseRequest/Request/RequestParser.hpp"
#include "monitorCgi.hpp"

#define CLIENT_TIMEOUT 60

using namespace std;

enum clinetState
{
    BEGIN,
    REQUEST_MODE,
    START_RESPOND,
    CGI_START,
    CGI_RUNING,
    CGI_END,
    RESPOND_MODE,
    LAST_CHUNKED,
    AUTO_INDEX_DONE,
    CONNECTION_CLOSED
};

struct bodyPlaceEnum
{
    enum place
    {
        NONE,
        RAM,
        DISK,
        AUTO_INDEX
    };
};

class clsClient
{
private:
    char ClientIp[INET_ADDRSTRLEN];
    std::string _serverPort;
    int _socket;
    int _fdRespond;
    short bytesToSend;
    ssize_t bodyLimit;
    long _LastConnection; // update connection in ms
    long _FirstConnection; // first established connection in ms mile seconds
    stPollRequest _dataForReq;
    PollOfClient _theData;
    sockaddr_in _addr;       // ip and port of the client
    clsServerConfig *block;
    RequestHandler _RequestXconfig;
    RequestParser _Requester;
    clsMainProcess _ResponderProecss;
    clinetState _state;
    bodyPlaceEnum::place _BodyPlace;
    clsMonitorCGI _monitorCGI;

    void _SendRespond(clsResponse &_Responder);
    int _ReadDataForReq();
    short _addSizeChunkToStr();
    void _initalizeRespondBuffer();
    void _LoadAutoIndex( clsResponse &_Responder);
    bool _handleInternal();

public:
    clsClient();
    void initializeClient(const sockaddr_in &addr, int fd, clsServerConfig *block, uint16_t portServer);
    void freeRessources();

    ~clsClient();

    // geters
    const clinetState &GetState() const;
    unsigned short GetPort() const;
    unsigned int GetIp() const;
    long GetTimeConnection() const;
    long GetLastConnection() const;
    int getPipeCgi();


    // seter
    void SetState(clinetState state);

    // methods
    void UpdateTime(); // update last connection
    void ResetAll();   // will reset all things

    void ProcessRequest();
    void ProcessRespond();
    void ProcessBoth(uint32_t events);
    bool monitorCgi();
    void logs();
    bool timeoutCgi();
    void initializeCGI();

};

#endif