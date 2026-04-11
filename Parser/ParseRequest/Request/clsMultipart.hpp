#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <set>

using namespace std;

enum whereAmI
{
    BOUNDARY,
    HEADER,
    END_HEADER,
    BODY,
};

enum whichBound
{
    None,
    startBoundary,
    midBoundary,
    endBoundary,
};

class clsMultiHeader
{
    private:
    #include <set>

        static std::set<char> forbidden;
        char Header[4000];
        int ofset;
        bool isError;
        string fileName;
        string contentType;
        bool contentDispoEx;
        bool nameVarEx;
    public:
        bool moveOffsetToDel(int &cur, int &trav);
        void addChar(char c);
        void Parsing();
        string getFileName();
        string getContentType();
        bool getError();
        void Reset();
};

class clsMultiPart
{
    private:
        clsMultiHeader multiHeaders;
        short lenBound; // at start initialize
        vector <string> filesNames;
        size_t trav;
        size_t cur;
        whereAmI processIn;
        char boundary[74];
        char endNormal[2];
        char endFinal[2];
        bool _hitEnd; // at start false
        bool foundFirstBound; // at start false
        bool error = false;
    public:
        void InitializeMulti(char *boundary, short lenBound, size_t start);
        whichBound isBoundary(char *arr, bool edgeCase = false);
        void Parser(char *arr, size_t &offset);
        bool getError();
        size_t getTrav() {return trav;}
        void setTrav(size_t t) { trav = t; };
        bool hitEnd() { return hitEnd;};
};