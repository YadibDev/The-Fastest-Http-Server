#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <set>

#define CONTENT_TYPE 1
#define CONTENT_DISPO 2
#define NAME_PAR 3
#define FILENAME_PAR 4

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

    string fileNameValue;
    string contentDispoValue;
    string contentTypeValue;
    bool fileNameEx;
    bool contentDispoEx;
    bool contentTypeEx;
    bool nameVarEx;
    bool insideDisposition;

    void moveOffsetToDel(size_t &trav);
    void skipWhiteSpaces(const char *str, size_t &trav, size_t size);
    char myComparaison(int st_end_key[2], bool parms = 0);
    char sanitizeKey(char *arr, size_t start, size_t end, char del, int start_end[2]);
    char sanitizeVal(char *arr, size_t start, size_t end, int start_end[2]);
    void storeValue(int value_indexes[2], string &value);
    char sanitizeKeyAndValue(int st_end_key[2], int st_end_value[2], size_t start, size_t end, size_t delTot);
    char storeValues(int key_indexes[2], int value_indexes[2], short delTot);
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
    vector<string> filesNames;
    size_t trav;
    size_t cur;
    whereAmI processIn;
    char boundary[74];
    char endNormal[2];
    char endFinal[2];
    bool _hitEnd;         // at start false
    bool foundFirstBound; // at start false
    bool error;

public:
    void InitializeMulti(char *boundary, short lenBound, size_t start);
    whichBound isBoundary(char *arr, bool edgeCase = false);
    void Parser(char *arr, size_t &offset);
    bool getError();
    size_t getTrav() { return trav; }
    void setTrav(size_t t) { trav = t; };
    bool hitEnd() { return _hitEnd; };
};