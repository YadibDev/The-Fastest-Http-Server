#include "../../Utils/HelperFunctions.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include "cstring"
#include "../../Parser/ParseRequest/URI/NUriParser.hpp"
#include <sys/stat.h>

#ifndef CLS_AUTO_INDEX
#define CLS_AUTO_INDEX

using namespace std;

enum flowAutoIndex
{
    START,
    TRY,
    CAN_PUSH,
    I_CANT_PUSH,
    END_TAG,
    LAST_CHUNKED_AUTO,
    DONE_AUTO_INDEX,
    ERROR_AUTO_INDEX
};

class clsAutoIndex
{
private:
    short _errorCode;
    static string headAutoIndexPreffix;
    static string headAutoIndexSuffix;
    static string endAutoIndex;
    static string preffRow;
    static string preffCol;
    static string suffCol;
    static string suffRow;

    DIR *streamDir;
    dirent *ptrDir;
    const char *physicalDir;
    const char *dir;
    short physicalDirSize;
    short dirSize;
    char *ptrSize;
    flowAutoIndex flowEnum;

    short _pushHeaderAutoIndex(char *buffer, short &start);
    flowAutoIndex _canPush(short nameSize, short availableSize);
    flowAutoIndex _insertRow(char *buffer, short &start, short &pushbytes, short limitSize);
    short _pushNewRow(char *buffer, short &start, char *file, unsigned char FILE_TYPE);
    short addDataToChar(char *buffer, const char *src, short &start, short len, bool encode = false);
    short _pushLink(char *buffer, short &start, char *file, unsigned char FILE_TYPE);
    short _pushTypeFile(char *buffer, short &start, unsigned char FILE_TYPE);
    void _addSizeChunk(short pushBytes, bool lastChunk = false);
    short _closeAutoIndex(char *buffer, short &start, short sizeLeft);


public:
    clsAutoIndex();
    ~clsAutoIndex();
    flowAutoIndex initializeAutoIndex(const char *physicalDir, const char *dir, short dirPhysicalSize, short dirSize);
    flowAutoIndex insertAutoDirective(char *buffer, short &start, short limitSize);
    short getErrorCode();
};

#endif