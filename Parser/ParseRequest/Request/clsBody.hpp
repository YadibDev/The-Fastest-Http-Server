#ifndef CLSBODY_HPP
#define CLSBODY_HPP

#include <iostream>
#include <string>
#include "clsMultipart.hpp"
#include "Header.hpp"

class clsRequest;

enum bodyPlace
{
    NONE,
    RAM,
    DISK
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
    char chunkMulti[8000];
    size_t multiLength;
    void Reset()
    {
        size = 0;
        cur = 0;
        trav = 0;
        readsize = true;
        multiLength = 0;
    }
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
    void bodyHandler(size_t &offset);
    void normalBody(size_t &offset);
    void Reset();
    void moveOffsetMulti();
    void handleMultiChunk(size_t &t, size_t offset, size_t &size, char *io_chunk);

};

#endif