#ifndef ___CLIENT_HPP___
#define ___CLIENT_HPP___

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include "../Utils/HelperFunctions.hpp"
#include "../PartRespond/mainprocess/Webserv.hpp"
#include "../Parser/RequestHandler/RequestHandler.hpp"
#include "../Parser/RequestHandler/ProcessRequestHandler.hpp"
#include "../Parser/ParseRequest/Request/RequestParser.hpp"

using namespace std;

enum clinetState
{
    BEGIN,
    REQUEST_MODE,
    START_RESPOND,
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
    // request object it will be here
    clsServerConfig &block;
    const int _socket;
    const size_t _FirstConnection; // first established connection in ms mile seconds
    const sockaddr_in _addr;       // ip and port of the client

    stPollRequest _dataForReq;
    PollOfClient _theData;
    bool _resetReq;

    string respondBuffer;

    RequestHandler RequestXconfig;
    RequestParser _Requester;
    clsMainProcess _ResponderProecss;

    clinetState _state;
    int _fdRespond;
    bodyPlaceEnum::place _BodyPlace;

    size_t _LastConnection; // update connection in ms

    void _SendRespond(const clsResponse &_Responder);
    int _ReadDataForReq();

public:
    clsClient(const sockaddr_in &addr, int fd, clsServerConfig &block); // initialize_state_by_begin
    clsClient(const clsClient &other);

    ~clsClient();

    // geters
    const clinetState &GetState() const;
    unsigned short GetPort() const;
    unsigned int GetIp() const;
    size_t GetTimeConnection() const;
    size_t GetLastConnection() const;

    // seter
    void SetState(clinetState state);

    // methods
    void UpdateTime(); // update last connection
    void ResetAll();   // will reset all things

    // the flow of request and respnd
    void ProcessRequest(); // will be
    void ProcessRespond();
    // void ProcessBoth();
};

#endif