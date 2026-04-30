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
    CONNECTION_CLOSED
};

struct bodyPlaceEnum
{
    enum place
    {
        NONE,
        RAM,
        DISK
    };
};

class clsClient
{
private:
    int _socket;
    int _fdRespond;
    ssize_t bytesToSend;
    ssize_t bodyLimit;
    size_t _LastConnection; // update connection in ms
    size_t _FirstConnection; // first established connection in ms mile seconds
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

    void _SendRespond(const clsResponse &_Responder);
    int _ReadDataForReq();
    ssize_t _addSizeChunkToStr();
    void _initalizeRespondBuffer();
    short    _ReadData(int fd);

public:
    clsClient();
    void initializeClient(const sockaddr_in &addr, int fd, clsServerConfig *block); // initialize_state_by_begin
    void freeRessources();

    ~clsClient();

    // geters
    const clinetState &GetState() const;
    unsigned short GetPort() const;
    unsigned int GetIp() const;
    size_t GetTimeConnection() const;
    size_t GetLastConnection() const;
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