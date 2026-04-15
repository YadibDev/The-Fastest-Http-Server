#ifndef CLSBODY_HPP
#define CLSBODY_HPP

#include <iostream>
#include <string>
#include "clsMultipart.hpp"
#include "Header.hpp"

class clsRequest;

struct bodyPlace
{
    enum body
    {
        NONE,
        RAM,
        DISK
    };
};


struct bodySteps
{
    enum step
    {
        SETTING_VARS,
        READING_HEADERS,
        READING_BODY,
        DONE_GOOD,
        DONE_WIHTERROR
    };
};

struct chunkVars
{
    uint16_t size;
    uint16_t cur;
    uint16_t trav;
    bool readsize;
    char chunkMulti[8000];
    uint16_t multiLength;
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
    void _handleChunk(size_t &ofset);
public:
    // geters
    clsBody(stPollRequest &p);
    const std::string &getFileName() const;
    const char *getBodyInRam() const;
    const bodyPlace &getBodyLocation() const; // is in ram or disk
    const bool &getIsError() const;
    const bodySteps::step getState() const;
    // methods
    bool thereIsAline(const std::string &buffer, size_t &start, char c = '\n', char after = '\r');
    void bodyHandler(uint16_t *off);
    void normalBody(uint16_t &offset);
    void Reset();
    void moveOffsetMulti();
    void handleMultiChunk(uint16_t &t, uint16_t offset, uint16_t &size, char *io_chunk);

};

#endif