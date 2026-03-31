#ifndef ___CLIENT_HPP___
#define ___CLIENT_HPP___

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include "../Utils/HelperFunctions.hpp"
#include "../PartRespond/mainprocess/Webserv.hpp"
#include "../Parser/ParseRequest/Request/Request.hpp"
#include "../Parser/RequestHandler/RequestHandler.hpp"
#include "../Parser/RequestHandler/ProcessRequestHandler.hpp"

using namespace std;
// new structs start
struct s_header_slot
{
    // s_view        key;
    // s_view        val;
    uint8_t        next; // char for -1
    uint32_t    Hash;

    // s_header_slot() : next(HttpTables::INVALID_INDEX) {}
};

struct PollOfClient {
    char request_metadata[16384];

    char io_chunk[8192];

    s_header_slot known_headers[10];
    s_header_slot unknown_headers[25];

    char Response_metadata[16384]; 

    uint16_t read_offset;
    uint16_t read_body;
    void Reset()
    {
        read_offset = 0;
        read_body = 0;
    }
};

struct stPollRequest
{
    char            *request_metadata;

    s_header_slot    *known_headers;
    s_header_slot    *unknown_headers;

    uint8_t            sizeUnknownHeaders; // maybe i will change it to const

    char            *io_chunk;
};
// new structs end

enum clinetState
{
    BEGIN,
    REQUEST_MODE,
    START_RESPOND,
    RESPOND_MODE,
    LAST_CHUNKED,
    CONNECTION_CLOSED
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
    PollOfClient _theData;
    stPollRequest _dataForReq;
    bool _resetReq;

    string respondBuffer;

    RequestHandler RequestXconfig;
    clsMainProcess _ResponderProecss;
    clsRequest _Requester;

    clinetState _state;
    int _fdRespond;
    whereIsBody _BodyPlace;
    
    const sockaddr_in _addr;       // ip and port of the client
    size_t _LastConnection;        // update connection in ms
    const size_t _FirstConnection; // first established connection in ms mile seconds
    const int _socket;
    
    void _SendRespond(const clsResponse &_Responder);
    int _ReadDataForReq();
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
    void ProcessRespond(const clsServerConfig &serverConfig);
    // void ProcessBoth();
};

#endif