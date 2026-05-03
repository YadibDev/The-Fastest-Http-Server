#include "../Utils/HelperFunctions.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include "cstring"
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
    DONE
};

class clsAutoIndex
{
private:
    static string headAutoIndexPreffix = "<!DOCTYPE html><style>table{font-size:20px;font-family:arial,sans-serif;border-collapse:collapse;}td,th{text-align:left;overflow:hidden;text-overflow:ellipsis;padding:10px;max-width:170px;}</style><body><h1>Index of ";
    static string headAutoIndexSuffix = "</h1><hr><table><tr><th>Name</th><th>file type</th></tr>";
    static string endAutoIndex = "</table></body>";
    static string preffRow = "<tr>";
    static string preffCol = "<td>";
    static string suffCol = "</td>";
    static string suffRow = "</tr>";

    DIR *streamDir;
    dirent *ptrDir;
    const char *physicalDir;
    const char *dir;
    short physicalDirSize;
    short dirSize;
    flowAutoIndex flowEnum = START;
public:
    void initializeAutoIndex(const char *physicalDir, const char *dir, short dirPhysicalSize, short dirSize);
    
}

#endif