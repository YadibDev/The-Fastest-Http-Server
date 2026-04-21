#ifndef CLSBODY_HPP
#define CLSBODY_HPP

#include <iostream>
#include <string>
#include "clsMultipart.hpp"
#include "Header.hpp"
#include <unistd.h>

class clsRequest;



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
public: // time to debug
    stPollRequest &data;
    clsMultiPart _multipartLib;
    chunkVars chunkHelp;
    ssize_t writeSize;
    std::string _fileName;
    
    int fd;
    bool        _isError;
    ssize_t     _contentLength;
    bool _isMultiPart;
    bool _isChunk;
    void _handleChunk(uint16_t &ofset);
public:
    enum place
    {
        NONE,
        RAM,
        DISK
    };
    enum step
    {
        SETTING_VARS,
        READING_HEADERS,
        READING_BODY,
        DONE_GOOD,
        DONE_WIHTERROR
    };
    place _bodyLocation;
    step _state;

    // geters
    clsBody(stPollRequest &p);
    const std::string &getFileName() const;
    const char *getBodyInRam() const;
    place getBodyLocation() const; // is in ram or disk
    const bool &getIsError() const;
    step getState() const;
    void shiftingData(char *src, int offset, int sizeShift);
    // methods
    bool thereIsAline(const std::string &buffer, size_t &start, char c = '\n', char after = '\r');
    void bodyHandler(uint16_t *off);
    void ParseBody(uint16_t &offset);
    ssize_t getBodySize();
    void StoreNormalBodyInDisk(uint16_t &offset);
    void Reset();
    void moveOffsetMulti(uint16_t &offset);

   void readSizeChunk(uint16_t &ofset, bool &error);
    void handleMultiChunk(uint16_t &t, uint16_t offset, uint16_t &size, char *io_chunk);

};

#endif