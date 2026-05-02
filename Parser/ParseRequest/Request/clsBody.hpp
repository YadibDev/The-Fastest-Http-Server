#ifndef CLSBODY_HPP
#define CLSBODY_HPP

#include <iostream>
#include <string>
#include "Header.hpp"
#include <cstdio>
#include <unistd.h>
#include "../../../Utils/HelperFunctions.hpp"
class clsRequest;



struct chunkVars
{
    short size;
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
    const std::string *uploadStore;
    HttpError _errorPage;
    int fd;
    long    _contentLength;
    bool _isChunk;
    void _handleChunk(uint16_t &ofset);
    bool readSizeChunk(uint16_t &ofset, bool &error, short &totRemoves);
    bool _saveChunkBody(uint16_t &ofset, bool &error, short &totRemoves);
    int _createUploadStoreFile();

    bool removeFile;
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
    ~clsBody();
    const std::string &getFileName() const;
    step getState() const;
    void shiftingData(char *src, int offset, int sizeShift);
    bool bodyHandler(uint16_t *off, const size_t &maxBodySize, bool isCgi, const char *path = NULL);
    void ParseBody(uint16_t &offset, const size_t &maxBodySize);
    ssize_t getBodySize();
    void StoreNormalBodyInDisk(uint16_t &offset);
    void Reset(); 
    void setUploadStore(const std::string *ptr);
    HttpError getError();

};

#endif