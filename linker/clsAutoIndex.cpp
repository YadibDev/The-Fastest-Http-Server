
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include "Utils/HelperFunctions.hpp"




std::string createColumnName(char *fileName, char *path)
{
    return std::string("<td><a href='") + path + "'>" + fileName + "</a></td>";
}

std::string createNormalColumn(char *buffer)
{
    return std::string("<td>") + buffer + "</td>";
}

std::string createRow(char *fileName, char *path, char *size, char *data)
{
    std::string result;

    result += "<tr>";
    result += createColumnName(fileName, path);
    result += "</tr>";
    return result;
}





flowAutoIndex canPush(short pathSize, short nameSize, short availableSize)
{
    short result;
    result = (pathSize + nameSize) * 3 + nameSize + 14 + 3 + 10 + 5 * 2 + 10; // is hardcode NO!!!!
    if (result > availableSize)
        return I_CANT_PUSH;
    else
        return CAN_PUSH;
}

int is_unreserved(char c)
{
    return (
        (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') ||
        c == '-' || c == '.' || c == '_' || c == '~') || 
        c == '/' || c == '&';
}

int addDataToChar(char *buffer, const char *src, short &start, short len, bool encode = false)
{
    if (encode)
    {
        short oldStart = start;
        for (int i = 0; i < len; i++)
        {
            if (is_unreserved(src[i]))
                buffer[start++] = src[i];
            else
            {
                buffer[start++] = '%';
                int temp = (int)src[i];
                if (temp < 16)
                    buffer[start++] = '0';
                string hexa = HelperFunctions::Convert_Hex("0123456789abcdef", temp);
                memcpy(&buffer[start], hexa.c_str(), hexa.size());
                start += hexa.size();
            }
        }
        return start - oldStart;
    }
    else
    {
        memcpy(&buffer[start], src, len);
        start += len;
    }
    return len;
}

short pushHeaderAutoIndex(char *buffer, short &start, char *dir)
{
    int result = 0;
    result += addDataToChar(buffer, headAutoIndexPreffix.c_str(), start, headAutoIndexPreffix.size());
    result += addDataToChar(buffer, dir, start, strlen(dir));
    result += addDataToChar(buffer, headAutoIndexSuffix.c_str(), start, headAutoIndexSuffix.size());
    return result;
}

short pushLink(char *buffer, short &start, char *dir, char *file, unsigned char FILE_TYPE)
{
    int result = 0;
    int fileLen = strlen(file);
    int dirLen = strlen(dir);

    result += addDataToChar(buffer, "<a href='", start, 9);
    result += addDataToChar(buffer, dir, start, dirLen, true);
    result += addDataToChar(buffer, file, start, fileLen, true);
    if (FILE_TYPE == DT_DIR)
        result += addDataToChar(buffer, "/", start, 1);
    result += addDataToChar(buffer, "'>", start, 2);
    result += addDataToChar(buffer, file, start, fileLen);
    result += addDataToChar(buffer, "</a>", start, 4);
    return result;
}

short pushTypeFile(char *buffer, short &start, unsigned char FILE_TYPE)
{
    char *ptr;
    if (FILE_TYPE == DT_REG)
        ptr = "FILE";
    else if (FILE_TYPE == DT_DIR)
        ptr = "DIRECTORY";
    else
        ptr = "UNKNONW";
    
    return addDataToChar(buffer, ptr, start, strlen(ptr));
}

short pushNewRow(char *buffer, short &start, char *dir, char *file, unsigned char FILE_TYPE)
{
    int result = 0;

    result += addDataToChar(buffer, preffRow.c_str(), start, preffRow.size());

    result += addDataToChar(buffer, preffCol.c_str(), start, preffCol.size());
    result += pushLink(buffer, start, dir, file, FILE_TYPE);
    result += addDataToChar(buffer, suffCol.c_str(), start, suffCol.size());

    result += addDataToChar(buffer, preffCol.c_str(), start, preffCol.size());
    result += pushTypeFile(buffer, start, FILE_TYPE);
    result += addDataToChar(buffer, suffCol.c_str(), start, suffCol.size());

    result += addDataToChar(buffer, suffRow.c_str(), start, suffRow.size());

    return result;
}


flowAutoIndex insertAutoDirective(char *buffer, short &start, short limitSize, char *dir)
{
    static DIR *streamDir = opendir(dir);
    static dirent *ptrDir = NULL;
    char *ptrSize = NULL;
    short pushbytes = 0;

    if (flowEnum == start)
    {
        ptrSize = &buffer[start];
        start += 6;
        pushbytes += pushHeaderAutoIndex(buffer, start, dir);
        flowEnum = TRY;
    }
    else if (flowEnum == I_CANT_PUSH)
    {
        std::cout << "last event is i can't push\n"
                  << std::endl;
        flowEnum = canPush(strlen(dir), strlen(ptrDir->d_name), limitSize - pushbytes - 6);
        exit(flowEnum == CAN_PUSH);
    }

    if (flowEnum == TRY)
    {
        flowEnum = CAN_PUSH;
        while ((ptrDir = readdir(streamDir)) != NULL && flowEnum == CAN_PUSH)
        {
            flowEnum = canPush(strlen(dir), strlen(ptrDir->d_name), limitSize - pushbytes - 6);
            if (flowEnum == I_CANT_PUSH)
                break;
            pushbytes += pushNewRow(buffer, start, dir, ptrDir->d_name, ptrDir->d_type);
        }
        if (ptrDir == NULL)
            flowEnum = END_TAG;
    }

    if (pushbytes > 0)
    {
        // update offset buffer
    }
    return flowEnum;
}

int main()
{
    std::ofstream file("out");
    char arr[16384];

    short start = 0;
    
    insertAutoDirective(arr, start, 1000, "/");
    arr[start] = '\0';
    file << arr;
    start = 0;
    insertAutoDirective(arr, start, 1000, "/");
    arr[start] = '\0';
    file << arr;
}