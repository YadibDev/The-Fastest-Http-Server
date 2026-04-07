#include "clsMultipart.hpp"

// multi header funtions

void clsMultiHeader::addChar(char c)
{
    if (ofset == 4000)
    {
        isError = true;
        return ;
    }
    Header[ofset++] = c;
};
void clsMultiHeader::Parsing()
{
    cout << "Display beta parsing\n";
    for (int i = 0; i < ofset; i++)
        cout << Header[i];
    cout << "\n----------------\n";
};
string clsMultiHeader::getFileName()
{
    return fileName;
};
string clsMultiHeader::getContentType()
{
    return contentType;
};
bool clsMultiHeader::getError()
{
    return isError;
};
void clsMultiHeader::Reset()
{
    ofset = 0;
    isError = false;
};

// multipart functions

void clsMultiPart::InitializeMulti(char *boundary, short lenBound, size_t start)
{
    this->lenBound = lenBound;
    std::memcpy(this->boundary, boundary, lenBound);
    std::memcpy(endNormal, "\r\n", 2);
    std::memcpy(endFinal, "--", 2);
    hitEnd = false;
    foundFirstBound = false;
    cur = start;
    trav = cur;
    processIn = HEADER;
    multiHeaders.Reset();
}

whichBound clsMultiPart::isBoundary(char *arr, bool edgeCase)
{
    if (foundFirstBound == false || edgeCase == true)
    {
        if (strncmp(arr, boundary, lenBound) == 0)
        {
            arr += lenBound;
            if (strncmp(arr, "\r\n", 2) == 0)
            {
                foundFirstBound = true;
                return startBoundary;
            }
            else if (strncmp(arr, endFinal, 2) == 0)
                return endBoundary;
            else
                return None;
        }
    }
    else if (strncmp(arr, "\r\n", 2) == 0)
    {
        arr += 2;
        if (strncmp(arr, boundary, lenBound) == 0)
        {
            arr += lenBound;
            if (strncmp(arr, endFinal, 2) == 0)
                return endBoundary;
            else if (strncmp(arr, endNormal, 2) == 0)       
                return midBoundary;
            else
                return None;
        }
    }
    return None;
};

void clsMultiPart::Parser(char *arr, size_t offset)
{
    while (trav < offset && cur < offset && error == false)
    {
        if (hitEnd)
            trav++;
        else if (foundFirstBound && arr[trav] != '\r')
        {
            if (processIn == HEADER)
            {
                multiHeaders.addChar(arr[trav]);
                if (multiHeaders.getError() == true)
                {
                    error = true;
                    return ;
                }
            }
            // else if (processIn == BODY);
                // write into the right file;
                // tobe continue
                // i must link headers with real path and and so forth
            trav++;
        }
        else
        {
            if (offset - trav >= lenBound + 2)
            {
                bool edgeCase = false;
                if (processIn == HEADER && strncmp("\r\n\r\n", &arr[trav], 4) == 0)
                {
                    edgeCase = true;
                    processIn = END_HEADER;
                    trav += 4;
                }
                if ((edgeCase == false && offset - trav < lenBound + 4) || (edgeCase && offset - trav < lenBound + 2))
                    return ;
                whichBound boundType = isBoundary(&arr[trav], edgeCase);
                if (boundType != None)
                {
                    if (processIn == BODY);
                        // reset body data
                    if (boundType == endBoundary)
                        hitEnd = true;
                    else if (boundType == startBoundary)
                    {
                        edgeCase = true; 
                        foundFirstBound = true;
                    }
                    processIn = HEADER;
                    trav += lenBound + 2 + !edgeCase * 2;
                }
                else if (edgeCase == false)
                {
                    if (processIn == HEADER)
                        multiHeaders.addChar(arr[trav]);
                    else;
                        // handleBody
                    trav += 1;
                }
            }
            else
                return ;
            if (processIn == END_HEADER)
            {
                multiHeaders.Parsing();
                if (multiHeaders.getError())
                {
                    error = true;
                    return ;
                }
                processIn = BODY;
            }
        }
    }
}
bool clsMultiPart::getError()
{
    return this->error;
}