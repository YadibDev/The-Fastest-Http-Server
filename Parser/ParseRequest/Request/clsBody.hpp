#ifndef CLSBODY_HPP
#define CLSBODY_HPP

#include <iostream>
#include <string>
#include "clsMultipart.hpp"
class clsRequest;

enum bodyPlace
{
    NONE,
    RAM,
    DISK
};

enum whichBoundary
{
    startBoundary,
    endBoundary,
    nothing
};

enum bodySteps
{
    SETTING_VARS,
    READING_HEADERS,
    READING_BODY,
    DONE_GOOD,
    DONE_WIHTERROR,
};

struct chunkVars
{
    size_t size;
    size_t cur;
    size_t trav;
    bool readsize;
    void Reset()
    {
        size = 0;
        cur = 0;
        trav = 0;
        readsize = true;
    }
};


struct stPollRequest
{
    char            *request_metadata;

    // s_header_slot    *known_headers;
    // s_header_slot    *unknown_headers;

    uint8_t            sizeUnknownHeaders; // maybe i will change it to const

    char            *io_chunk;
};

class clsBody
{
private:
    stPollRequest &data;
    clsMultiPart _multipartLib;
    chunkVars chunkHelp;
    bodyPlace _bodyLocation;
    bodySteps _state;
    std::string _fileName;
    
    int fd;
    bool        _isError;
    ssize_t     _Length;
    bool _isMultiPart;
    bool _isChunk;
    void _handleChunk(size_t ofset);
public:
    // geters
    clsBody(stPollRequest &p);
    const std::string &getFileName() const;
    const char *getBodyInRam() const;
    const bodyPlace &getBodyLocation() const; // is in ram or disk
    const bool &getIsError() const;
    const bodySteps &getState() const;
    // methods
    bool thereIsAline(const std::string &buffer, size_t &start, char c = '\n', char after = '\r');
    void bodyHandler(const std::string &buffer, clsRequest &request);
    void normalBody(const char *, ssize_t nBytes);
    void Reset();

};

#endif