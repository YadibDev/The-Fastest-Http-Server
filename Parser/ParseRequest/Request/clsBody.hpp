#ifndef CLSBODY_HPP
#define CLSBODY_HPP

#include <iostream>
#include <string>
#include "Header.hpp"
#include <unistd.h>
#include "../../../Utils/HelperFunctions.hpp"
class clsRequest;



struct chunkVars
{
    uint16_t size;
    uint16_t cur;
    uint16_t trav;
    bool readsize;
    void Reset()
    {
        size = 0;
        cur = 0;
        trav = 0;
        readsize = true;
    }
};



class clsBody
{
public: // time to debug
    stPollRequest &data;
    chunkVars chunkHelp;
    long writeSize;
    std::string _fileName;
    int fd;
    int  _isError;
    long    _contentLength;
    bool _isChunk;
    void _handleChunk(uint16_t &ofset);
public:
    enum step
    {
        SETTING_VARS,
        READING_BODY,
        DONE_GOOD,
        DONE_WIHTERROR
    };
    step _state;

    // geters
    clsBody(stPollRequest &p);
    const std::string &getFileName() const;
    const int &getIsError() const;
    step getState() const;
    void shiftingData(char *src, int offset, int sizeShift);
    // methods
    bool bodyHandler(uint16_t *off, const size_t &maxBodySize);
    void ParseBody(uint16_t &offset, const size_t &maxBodySize);
    ssize_t getBodySize();
    void StoreNormalBodyInDisk(uint16_t &offset);
    void Reset(); 
    // void moveOffsetMulti(uint16_t &offset);
   void readSizeChunk(uint16_t &ofset, bool &error);

};

#endif