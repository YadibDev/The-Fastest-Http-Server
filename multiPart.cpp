// #include <cstdlib>
// #include <cstring>
// #include <string>
// #include <iostream>

// using namespace std;

// enum whereAmI
// {
//     BOUNDARY,
//     HEADER,
//     END_HEADER,
//     BODY,
// };

// enum whichBound
// {
//     None,
//     startBoundary,
//     midBoundary,
//     endBoundary,
// };

// #include <vector>

// // void multipart(int ofset)
// // {
// //     static char *boundary = "--MyBoundary\r\n";
// //     static char *endNormal = "\r\n";
// //     static char *endFinal = "--\r\n";
// //     static int sizeBoundary = 12;
// //     static int c = 0;
// //     static int t = 0;
// //     static whereAmI step = firstMulti;
// //     static bool endBoundary = false;
// //     static int start = 0;
// //     static int end = 0;
// //     bool keepUp = true;
// //     while (keepUp && c < ofset && t < ofset)
// //     {
// //         if (step == firstMulti)
// //         {
// //             if (ofset - t >= sizeBoundary + 2)
// //             {
// //                 if (strncmp(boundary, &req.c_str()[t], sizeBoundary + 2) == 0)
// //                 {
// //                     t += sizeBoundary + 2;
// //                     boundary = "\r\n--MyBoundary";
// //                     sizeBoundary = 14;
// //                     step = Header;
// //                 }
// //                 else
// //                     t++;
// //                 c = t;
// //             }
// //             else
// //                 break;
// //         }
// //         else if (ofset - t >= sizeBoundary)
// //         {
// //             while ((step == Header || step == Body) && ofset - t >= sizeBoundary)
// //             {
// //                 if (req[t] != '\r')
// //                 {
// //                     // if (step == body)
// //                         // add algo here
// //                     t++;
// //                 }
// //                 else
// //                 {
// //                     if (strncmp(boundary, &req.c_str()[t], sizeBoundary) == 0)
// //                     {
// //                         end = t;
// //                         int temp = t + sizeBoundary;
// //                         if (ofset - temp >= 2 && strncmp(&req.c_str()[temp], endNormal, 2) == 0) // normal boundary
// //                         {
// //                             if (step == Header)
// //                                 step = EndHeader;
// //                             else if (step == Body)
// //                                 step = EndBody;
// //                             t += temp + 2;
// //                         }
// //                         else if (ofset - temp >= 4 && strncmp(&req.c_str()[temp], endFinal, 4) == 0) // end boundary
// //                         {
// //                             if (step == Header)
// //                                 step = EndHeader;
// //                             else if (step == Body)
// //                                 step = EndBody;
// //                             endBoundary = true;
// //                             t += temp + 4;
// //                         }
// //                         else if (ofset - (temp) >= 4)
// //                         {
// //                             // if (step == body)
// //                                 // add algo body here
// //                             t++;
// //                         }
// //                         else
// //                         {
// //                             cout << "hello world\n";
// //                             keepUp = false;
// //                             break ;
// //                         }
// //                     }
// //                     else if (step == Header && strncmp("\r\n\r\n", &req.c_str()[t], 4) == 0)
// //                     {
// //                         end = t;
// //                         t += 4;
// //                         step = EndHeader;
// //                     }
// //                     else if (step == Body)
// //                     {
// //                         t++;
// //                     }
// //                 }
// //             }
// //             if (step == EndHeader)
// //             {
// //                 cout << "-----------------\n";
// //                 cout << "HEADERS BY YADIB\n";
// //                 while (c < t)
// //                 {
// //                     cout << req[c++];
// //                 }
// //                 cout << "-----------------\n";
// //                 step = Body;
// //             }
// //             else if (step == EndBody)
// //             {
// //                 cout << "****************\n";
// //                 cout << "Body BY YADIB\n";
// //                 while (c < end)
// //                 {
// //                     cout << req[c++];
// //                 }
// //                 c = t;
// //                 cout << "**************\n";
// //                 step = Header;
// //             }
// //             if (endBoundary)
// //                 return ;
// //         }
// //         else
// //             break;
// //         }
// //     }

// class clsMultiHeader
// {
//     private:
//         char Header[4000];
//         int ofset;
//         bool isError;
//         string fileName;
//         string contentType;
//         bool contentDispoEx;
//         bool nameVarEx;
//     public:
//         void addChar(char c)
//         {
//             if (ofset == 4000)
//             {
//                 isError = true;
//                 return ;
//             }
//             Header[ofset++] = c;
//         };
//         void Parsing()
//         {
//             cout << "Display beta parsing\n";
//             for (int i = 0; i < ofset; i++)
//                 cout << Header[i];
//             cout << "\n----------------\n";
//         };
//         string getFileName() {return fileName; };
//         string getContentType() {return contentType; };
//         bool getError() { return isError; };
//         void Reset()
//         {
//             ofset = 0;
//             isError = false;
//         };
// };

// class clsMultiPart
// {
//     private:
//         clsMultiHeader multiHeaders;
//         short lenBound; // at start initialize
//         vector <string> filesNames;
//         size_t trav;
//         size_t cur;
//         whereAmI processIn;
//         char boundary[74];
//         char endNormal[2];
//         char endFinal[2];
//         bool hitEnd; // at start false
//         bool foundFirstBound; // at start false
//         bool error = false;
//     public:
//         clsMultiPart(char *boundary, short lenBound, size_t start)
//         {
//             this->lenBound = lenBound;
//             std::memcpy(this->boundary, boundary, lenBound);
//             std::memcpy(endNormal, "\r\n", 2);
//             std::memcpy(endFinal, "--", 2);
//             hitEnd = false;
//             foundFirstBound = false;
//             cur = start;
//             trav = cur;
//             processIn = HEADER;
//             multiHeaders.Reset();
//         }
//         whichBound isBoundary(char *arr, bool edgeCase = false)
//         {
//             if (foundFirstBound == false || edgeCase == true)
//             {
//                 if (strncmp(arr, boundary, lenBound) == 0)
//                 {
//                     arr += lenBound;
//                     if (strncmp(arr, "\r\n", 2) == 0)
//                     {
//                         foundFirstBound = true;
//                         return startBoundary;
//                     }
//                     else if (strncmp(arr, endFinal, 2) == 0)
//                         return endBoundary;
//                     else
//                         return None;
//                 }
//             }
//             else if (strncmp(arr, "\r\n", 2) == 0)
//             {
//                 arr += 2;
//                 if (strncmp(arr, boundary, lenBound) == 0)
//                 {
//                     arr += lenBound;
//                     if (strncmp(arr, endFinal, 2) == 0)
//                         return endBoundary;
//                     else if (strncmp(arr, endNormal, 2) == 0)
//                         return midBoundary;
//                     else
//                         return None;
//                 }
//             }
//             return None;
//         };
//         void Parser(char *arr, size_t offset)
//         {
//             while (trav < offset && cur < offset)
//             {
//                 if (hitEnd)
//                     trav++;
//                 else if (foundFirstBound && arr[trav] != '\r')
//                 {
//                     if (processIn == HEADER)
//                     {
//                         multiHeaders.addChar(arr[trav]);
//                         if (multiHeaders.getError() == true)
//                         {
//                             error = true;
//                             return ;
//                         }
//                     }
//                     else if (processIn == BODY);
//                         // tobe continue
//                         // i must link headers with real path and and so forth
//                     trav++;
//                 }
//                 else
//                 {
//                     if (offset - trav >= lenBound + 4)
//                     {
//                         bool edgeCase = false;
//                         if (processIn == HEADER && strncmp("\r\n\r\n", &arr[trav], 4) == 0)
//                         {
//                             edgeCase = true;
//                             processIn = END_HEADER;
//                             trav += 4;
//                         }
//                         whichBound boundType = isBoundary(&arr[trav], edgeCase);
//                         if (boundType != None)
//                         {
//                             if (processIn == BODY);
//                                 // reset body data
//                             if (boundType == endBoundary)
//                                 hitEnd = true;
//                             processIn = HEADER;
//                             trav += lenBound + 2 + edgeCase * 2;
//                         }
//                         else if (edgeCase == false)
//                         {
//                             if (processIn == HEADER)
//                                 multiHeaders.addChar(arr[trav]);
//                             else;
//                                 // handleBody
//                             trav += 1;
//                         }
//                     }
//                     else
//                         return ;
//                     if (processIn == END_HEADER)
//                     {
//                         multiHeaders.Parsing();
//                         if (multiHeaders.getError())
//                         {
//                             error = true;
//                             return ;
//                         }
//                         processIn = BODY;
//                     }
//                 }
//             }
//         }
//         bool getError() {return this->error;}
// };

#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

#include <set>

std::set<char> forbidden = {
    '(', ')', '<', '>', '@', ',', ';', ':',
    '\\', '"', '/', '[', ']', '?', '=', '{', '}'
};

std::string req =
    "--MyBoundary\r\n"
    "Content-Disposition: form-data; name=\"username\"\r\n"
    "\r\n"
    "yassine\r\n\r\n"
    "--MyBoundary--\r\n";

string Header = "a\r\n"
                "b\r\n"
                "Content-Disposition: form-data; name=\"username\"\r\n"
                "Content-type: \"image/png\"";

void moveOffsetToDel(size_t &trav)
{
    while (trav < Header.size())
    {
        if (Header[trav] == ';')
            return ;
        else if (trav + 1 < Header.size() && Header[trav] == '\r' && Header[trav + 1] == '\n')
            return ;
        trav++;
    }
}

void skipWhiteSpaces(const char *str, size_t &trav, size_t size)
{
    while (trav < size && (str[trav] == ' ' || Header[trav] == '\t'))
        trav++;
}

bool StoreValueAfterDelim(string &arr, size_t &trav, size_t &cur)
{
    short quotes = 0;
    bool backSlach = false;

        skipWhiteSpaces(Header.c_str(), trav, Header.size());
        while (trav < Header.size() && Header[trav] != '\r')
        {
            if ((arr.length() != 0 && quotes != 1 && forbidden.count(Header[trav])) || (Header[trav] >= 0 && Header[trav] <= 32) || Header[trav] == 127)
            {
                if (Header[trav] == ';')
                {
                    trav++;
                    break;
                }
                else if (Header[trav] == ' ' || Header[trav] == '\t')
                    skipWhiteSpaces(Header.c_str(), trav, Header.size());
                else
                    return false;
            }
            else
            {
                if (backSlach == false && Header[trav] == '"')
                {
                    if (arr.length() == 0 || quotes == 1)
                        quotes++;
                    else
                        return false;
                    trav++;
                }
                else
                {
                    if (backSlach == false && Header[trav] == '\\')
                        backSlach = true;
                    else if (quotes != 2)
                    {
                        arr += Header[trav++];
                        backSlach = false;
                    }
                    else
                        return false;
                }
            }
        }
        if (quotes == 1)
            return 1;
        cur = trav;
        return true;
}
char sanitizeKey(char *arr, size_t start, size_t end)
{
    for (int i 
    return 'S'
    return 'E';
}

int main()
{
    size_t cur = 0;
    size_t trav = 0;
    char option = 'k';

    bool contentTypeEx = false;
    bool fileNameEx = false;
    bool contentDispoEx = false;
    bool nameVarEx = false;
    
    string contentType = "";
    string contentDispo = "";

    while (1)
    {
        moveOffsetToDel(trav);
        if (option = 'k')

    }
    cout << contentType << std::endl;
    cout << contentDispo << std::endl;
    // "Content-Disposition";
    // "content-type";

}
    // clsMultiPart multiParser("--MyBoundary", 12, 0);

    // multiParser.Parser(&req[0], 11);
    // multiParser.Parser(&req[0], 11);
    // multiParser.Parser(&req[0], 11);
    // multiParser.Parser(&req[0], 50);
    // multiParser.Parser(&req[0], 58);
    // multiParser.Parser(&req[0], 92);
    // std::cout << "_______\n";


// int main(){
//     cout <<  "{"<< &req[72] << std::endl;
//     cout <<  "{"<< req.size() << std::endl;
//     cout <<  "{"<< (int)req[72] << "}"<< std::endl;

//     multipart(10);
//     cout << "+\n";
//     multipart(46);
//     cout << "+\n";
//     multipart(91);
//     multipart(92);
// }