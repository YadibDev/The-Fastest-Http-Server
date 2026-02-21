#ifndef ___CLIENT_HPP___
#define ___CLIENT_HPP___

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include "../Utils/HelperFunctions.hpp"
#include "../PartRespond/mainprocess/Webserv.hpp"

using namespace std;

enum clinetState
{
    BEGIN,
    REQUEST_MODE,
    RESPOND_MODE,
    SEND_BODY,
    LAST_CHUNKED,
};

enum whereIsBody
{
    NONE,
    RAM,
    DISK_FILE
};

class clsClient
{
private:
    // request object it will be here
    size_t _BodyOfset;
    string _DataLeft;
    size_t _HeaderOfset;
    clsResponse _Responder;
    clinetState _state;
    int _fdRespond;
    whereIsBody _BodyPlace;
    const sockaddr_in _addr;       // ip and port of the client
    size_t _LastConnection;        // update connection in ms
    const size_t _FirstConnection; // first established connection in ms mile seconds
    const int _socket;

    void _SendRespond();
public:
    clsClient(const sockaddr_in &addr, int fd); // initialize_state_by_begin
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
    void ProcessBoth();
};

#endif