
#include "clsAutoIndex.hpp"

string clsAutoIndex::headAutoIndexPreffix = "<!DOCTYPE html><style>table{font-size:20px;font-family:arial,sans-serif;border-collapse:collapse;}td,th{text-align:left;overflow:hidden;text-overflow:ellipsis;padding:10px;max-width:170px;}</style><body><h1>Index of ";
string clsAutoIndex::headAutoIndexSuffix = "</h1><hr><table><tr><th>Name</th><th>file type</th></tr>";
string clsAutoIndex::endAutoIndex = "</table></body>";
string clsAutoIndex::preffRow = "<tr>";
string clsAutoIndex::preffCol = "<td>";
string clsAutoIndex::suffCol = "</td>";
string clsAutoIndex::suffRow = "</tr>";

int is_unreserved(char c)
{
    return (
               (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
               (c >= '0' && c <= '9') ||
               c == '-' || c == '.' || c == '_' || c == '~') ||
           c == '/' || c == '&';
}

clsAutoIndex::clsAutoIndex()
{
    streamDir = NULL;
    ptrDir = NULL;
}

clsAutoIndex::~clsAutoIndex()
{
    if (streamDir)
        closedir(streamDir);
}

void clsAutoIndex::initializeAutoIndex(const char *physicalDir, const char *dir, short dirPhysicalSize, short dirSize)
{
    if (streamDir)
        closedir(streamDir);

    streamDir = NULL;
    ptrDir = NULL;
    this->physicalDir = physicalDir;
    this->dir = dir;
    this->physicalDirSize = dirPhysicalSize;
    this->dirSize = dirSize;
    flowEnum = START;
    ptrSize = NULL;
}

flowAutoIndex clsAutoIndex::_canPush(short nameSize, short availableSize)
{
    short result;
    result = (dirSize + nameSize) * 3 + nameSize + 14 + 3 + 10 + 5 * 2 + 10; // is hardcode NO!!!!
    if (result > availableSize)
        return I_CANT_PUSH;
    else
        return CAN_PUSH;
}

short clsAutoIndex::addDataToChar(char *buffer, const char *src, short &start, short len, bool encode)
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

short clsAutoIndex::_pushHeaderAutoIndex(char *buffer, short &start)
{
    int result = 0;
    result += addDataToChar(buffer, headAutoIndexPreffix.c_str(), start, headAutoIndexPreffix.size());
    result += addDataToChar(buffer, dir, start, dirSize);
    result += addDataToChar(buffer, headAutoIndexSuffix.c_str(), start, headAutoIndexSuffix.size());
    return result;
}

short clsAutoIndex::_pushLink(char *buffer, short &start, char *file, unsigned char FILE_TYPE)
{
    short result = 0;
    short fileLen = strlen(file);

    result += addDataToChar(buffer, "<a href='", start, 9);
    result += addDataToChar(buffer, dir, start, dirSize, true);
    result += addDataToChar(buffer, file, start, fileLen, true);
    if (FILE_TYPE == DT_DIR)
        result += addDataToChar(buffer, "/", start, 1);
    result += addDataToChar(buffer, "'>", start, 2);
    result += addDataToChar(buffer, file, start, fileLen);
    result += addDataToChar(buffer, "</a>", start, 4);
    return result;
}

short clsAutoIndex::_pushTypeFile(char *buffer, short &start, unsigned char FILE_TYPE)
{
    const char *ptr;
    if (FILE_TYPE == DT_REG)
        ptr = "FILE";
    else if (FILE_TYPE == DT_DIR)
        ptr = "DIRECTORY";
    else
        ptr = "UNKNONW";

    return addDataToChar(buffer, ptr, start, strlen(ptr));
}

short clsAutoIndex::_pushNewRow(char *buffer, short &start, char *file, unsigned char FILE_TYPE)
{
    short result = 0;

    result += addDataToChar(buffer, preffRow.c_str(), start, preffRow.size());

    result += addDataToChar(buffer, preffCol.c_str(), start, preffCol.size());
    result += _pushLink(buffer, start, file, FILE_TYPE);
    result += addDataToChar(buffer, suffCol.c_str(), start, suffCol.size());

    result += addDataToChar(buffer, preffCol.c_str(), start, preffCol.size());
    result += _pushTypeFile(buffer, start, FILE_TYPE);
    result += addDataToChar(buffer, suffCol.c_str(), start, suffCol.size());

    result += addDataToChar(buffer, suffRow.c_str(), start, suffRow.size());

    return result;
}

flowAutoIndex clsAutoIndex::_insertRow(char *buffer, short &start, short &pushbytes, short limitSize)
{
    flowEnum = _canPush(strlen(ptrDir->d_name), (limitSize - pushbytes - 8));
    if (flowEnum == I_CANT_PUSH)
        return flowEnum;
    pushbytes += _pushNewRow(buffer, start, ptrDir->d_name, ptrDir->d_type);
    return flowEnum;
}

void clsAutoIndex::_addSizeChunk(short pushBytes, bool lastChunk)
{
    std::string hexa = HelperFunctions::Convert_Hex("0123456789abcdef", pushBytes);

    if (ptrSize == NULL)
        return;
    if (lastChunk)
    {
        memcpy(ptrSize, "0\r\n\r\n", 5);
        flowEnum = DONE_AUTO_INDEX;
        ptrSize = NULL;
        return;
    }

    int size = 4 - hexa.size();
    memset(ptrSize, '0', size);
    memcpy(&ptrSize[size], hexa.c_str(), hexa.size());
    memcpy(&ptrSize[4], "\r\n", 2);
    std::cout << ptrSize << std::endl;
    ptrSize = NULL;
}

short clsAutoIndex::_closeAutoIndex(char *buffer, short &start, short sizeLeft)
{
    bool canPush = (sizeLeft >= this->endAutoIndex.size());

    if (canPush == false)
        return 0;

    if (ptrSize == NULL)
    {
        ptrSize = &buffer[start];
        start += 6;
    }
    flowEnum = LAST_CHUNKED;
    return addDataToChar(buffer, this->endAutoIndex.c_str(), start, this->endAutoIndex.size());
}

flowAutoIndex clsAutoIndex::insertAutoDirective(char *buffer, short &start, short limitSize)
{
    short pushbytes = 0;
    if (flowEnum == START)
    {
        streamDir = opendir(physicalDir);
        if (streamDir == NULL)
            return ERROR_AUTO_INDEX;

        // reserve 6 bytes for size chunk
        ptrSize = &buffer[start];
        start += 6;
        pushbytes += _pushHeaderAutoIndex(buffer, start);
        flowEnum = TRY;
    }
    else if (flowEnum == I_CANT_PUSH)
    {
        flowEnum = _canPush(strlen(ptrDir->d_name), (limitSize - pushbytes - 8));
        if (flowEnum == CAN_PUSH)
        {
            ptrSize = &buffer[start];
            start += 6;
            flowEnum = _insertRow(buffer, start, pushbytes, limitSize);
            flowEnum = TRY;
        }
        else
            return flowEnum;
    }
    if (flowEnum == TRY)
    {
        flowEnum = CAN_PUSH;
        while ((ptrDir = readdir(streamDir)) != NULL && flowEnum == CAN_PUSH)
        {
            flowEnum = _insertRow(buffer, start, pushbytes, limitSize);
        }
        if (ptrDir == NULL)
            flowEnum = END_TAG;
    }

    if (flowEnum == END_TAG)
    {
        pushbytes += _closeAutoIndex(buffer, start, limitSize - pushbytes - 8);
    }

    if (pushbytes)
    {
        memcpy(&buffer[start], "\r\n", 2);
        _addSizeChunk(pushbytes);
        start += 2;
        pushbytes += 6;
    }

    if (flowEnum == LAST_CHUNKED && limitSize - pushbytes - 5 >= 0)
    {
        ptrSize = &buffer[start];
        _addSizeChunk(0, true);
        start += 5;
    }
    return flowEnum;
}

// int addDataToChar(char *buffer, const char *src, short &start, short len, bool encode = false)
// {
//     if (encode)
//     {
//         short oldStart = start;
//         for (int i = 0; i < len; i++)
//         {
//             if (is_unreserved(src[i]))
//                 buffer[start++] = src[i];
//             else
//             {
//                 buffer[start++] = '%';
//                 int temp = (int)src[i];
//                 if (temp < 16)
//                     buffer[start++] = '0';
//                 string hexa = HelperFunctions::Convert_Hex("0123456789abcdef", temp);
//                 memcpy(&buffer[start], hexa.c_str(), hexa.size());
//                 start += hexa.size();
//             }
//         }
//         return start - oldStart;
//     }
//     else
//     {
//         memcpy(&buffer[start], src, len);
//         start += len;
//     }
//     return len;
// }

// short pushLink(char *buffer, short &start, char *dir, char *file, unsigned char FILE_TYPE)
// {
//     int result = 0;
//     int fileLen = strlen(file);
//     int dirLen = strlen(dir);

//     result += addDataToChar(buffer, "<a href='", start, 9);
//     result += addDataToChar(buffer, dir, start, dirLen, true);
//     result += addDataToChar(buffer, file, start, fileLen, true);
//     if (FILE_TYPE == DT_DIR)
//         result += addDataToChar(buffer, "/", start, 1);
//     result += addDataToChar(buffer, "'>", start, 2);
//     result += addDataToChar(buffer, file, start, fileLen);
//     result += addDataToChar(buffer, "</a>", start, 4);
//     return result;
// }

// short pushTypeFile(char *buffer, short &start, unsigned char FILE_TYPE)
// {
//     char *ptr;
//     if (FILE_TYPE == DT_REG)
//         ptr = "FILE";
//     else if (FILE_TYPE == DT_DIR)
//         ptr = "DIRECTORY";
//     else
//         ptr = "UNKNONW";

//     return addDataToChar(buffer, ptr, start, strlen(ptr));
// }

// short pushNewRow(char *buffer, short &start, char *dir, char *file, unsigned char FILE_TYPE)
// {
//     int result = 0;

//     result += addDataToChar(buffer, preffRow.c_str(), start, preffRow.size());

//     result += addDataToChar(buffer, preffCol.c_str(), start, preffCol.size());
//     result += pushLink(buffer, start, dir, file, FILE_TYPE);
//     result += addDataToChar(buffer, suffCol.c_str(), start, suffCol.size());

//     result += addDataToChar(buffer, preffCol.c_str(), start, preffCol.size());
//     result += pushTypeFile(buffer, start, FILE_TYPE);
//     result += addDataToChar(buffer, suffCol.c_str(), start, suffCol.size());

//     result += addDataToChar(buffer, suffRow.c_str(), start, suffRow.size());

//     return result;
// }

// flowAutoIndex insertAutoDirective(char *buffer, short &start, short limitSize, char *dir)
// {
//     static DIR *streamDir = opendir(dir);
//     static dirent *ptrDir = NULL;
//     char *ptrSize = NULL;
//     short pushbytes = 0;

//     if (flowEnum == start)
//     {
//         ptrSize = &buffer[start];
//         start += 6;
//         pushbytes += pushHeaderAutoIndex(buffer, start, dir);
//         flowEnum = TRY;
//     }
//     else if (flowEnum == I_CANT_PUSH)
//     {

//         exit(flowEnum == CAN_PUSH);
//     }

//     if (flowEnum == TRY)
//     {
//         flowEnum = CAN_PUSH;
//         while ((ptrDir = readdir(streamDir)) != NULL && flowEnum == CAN_PUSH)
//         {
//             flowEnum = canPush(strlen(dir), strlen(ptrDir->d_name), limitSize - pushbytes - 6);
//             if (flowEnum == I_CANT_PUSH)
//                 break;
//             pushbytes += pushNewRow(buffer, start, dir, ptrDir->d_name, ptrDir->d_type);
//         }
//         if (ptrDir == NULL)
//             flowEnum = END_TAG;
//     }

//     if (pushbytes > 0)
//     {
//         // update offset buffer
//     }
//     return flowEnum;
// }

int main()
{
    clsAutoIndex autoIndex;
    std::ofstream output("output.html");

    char buffer[16000];
    short start = 0;
    autoIndex.initializeAutoIndex("/goinfre/yadib/The-Fastest-Http-Server/linker", "The-Fastest-Http-Server/linker", 45, 30);
    flowAutoIndex flow;
    while ((flow = autoIndex.insertAutoDirective(buffer, start, 500)) < DONE_AUTO_INDEX)
    {
        buffer[start] = '\0';
        std::cout << start << std::endl;
        std::cout << buffer << std::endl;
        start = 0;
        output << buffer;
    }
    buffer[start] = '\0';
    output << buffer;

    if (flow == DONE_AUTO_INDEX)
        std::cout << "DONE_AUTO_INDEX" << std::endl;
}
