#ifndef HELPERFUNCTIONS_HPP
#define HELPERFUNCTIONS_HPP

#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <cstring>
#include "../Parser/ParseRequest//Request/HttpTypes.hpp"


struct stEventProcess
{
    enum eEventProcess {RUNINNG, THE_END, END_WITH_PARSE, END_WITH_TIMOUT = 504, END_UNKNOW = 500};
};
struct stEventData
{
    enum eEventData {STILL_EXIST, END_PIPE};
};

// achraf headers 



class HelperFunctions
{
public:
	static s_view  find_first_of_view(s_view view, const char* set);
	static s_view  extract_between(s_view view, const char* start_set, const char* end_set);
	static void skipWhitespace(const std::string &str, size_t &pos);
	static bool isCRLF(const std::string &str);
	static long hexToDec(const std::string &hex);
	static long	hexToDecS_view(const char *buf, int len);
	static bool is_numeric(const std::string &str);
	static bool is_CTLsString(const std::string &str);
	static bool checkIfTheFirstWord(std::string str, std::string Start, size_t POS);
	static std::string normalizeLWS(const std::string &input);
	static bool isLWS(char c);
	static bool myIsspace(std::string str, size_t pos);
	static std::vector<std::string> splitCommaSeparated(const std::string &value);
	static bool strIsSpace(const std::string &str);
	static bool isBoundary(const std::string &str, const std::string &boundary, std::string &remander);
	static short    isValidPath(const std::string& path, bool expectDir);
	static s_view find_last_of_view(s_view view, const char* set);
	static bool	joinArr(char *buffer, const char *AddStr, size_t size);
    static size_t join_views(char* dst, size_t dst_size, const s_view& v1, const s_view& v2);




    // achraf part
    static unsigned long getCurrentTimeInMs();
    static long getCurrentTimeInS();
	static bool CmpWord(const char *Str, const std::string &Word, short SizeStr);
    static size_t FindCRLF(const std::string &Str, const std::string &CRLF);
    static bool IsStringDigit(const std::string &StringDigit, short Start, short End);
    static bool Iswhaitspace(char C);
    static std::string TrimStr(std::string Str, const std::string &Sep);
    static void ConvertStringToLower(std::string &Str, short Size);
    static std::string ConvertStringToUpper(std::string &Str);
    static bool Ischar(const std::string &Sep, char C);
    static int SkeeSep(const std::string &Str, const std::string &Sep);
    static int SkeeSep(const std::string &Str, char Sep);
    static int ReadData(int FD, std::string &Data, ssize_t Size);
    static std::string GetNextLine(int FD, std::string &BigData, ssize_t Size);
    static std::string GTMHTTP(tm *GMT);
    static std::string DateTime();
    static std::string Convert_Hex(const std::string &Str, int Num);
    static char	*ft_strdup(const char *src);
    static void	free_matrex(char ***matrex, short IndexStart);
    static const char *GetTypeDataFile(const std::string &Str);
    static void GetCleanLineHeader(const char *BigData, std::string &CleanLine ,short &MaxSizeHeader, bool &Flag, short &i, short LengthData);
    static char	*ft_itoa(int n);
    static char	*ft_itoa_negative(int n, char *int_char);
    static int	len_int(int nb);
    static void	*ft_memset(void *str, int c, size_t n);
    static const char *GetType(const std::string &Type);
    static void StoredDefaultType();
    static void StoredBodys();
    static void StoredMessage();
    static const char *GetStatusMessage(int Status);
    static const char * GetBody(int Status);
    static bool ComparHead(const std::string &Str1, const std::string &Str2, short Start, short End);
    static void CopyStr(const std::string &Str_src, std::string &Str_new, short Start, short Pos);
    static void CopyStr(const char *Str_src, std::string &Str_new, short Start, short Length);
    static size_t	ft_strlen(const char *s);
    static short    LengthWord(const std::string &Str, const std::string &Sep, short Start);
    static int Countword(const std::string &Str, const std::string &Sep);
    static void NumToStr(int Number, std::string &Str);
    static stEventProcess::eEventProcess checkProcessStatus(int pid, int op = WNOHANG);
    static int SkeepAtLast(const std::string& Str, const std::string &Sep);
    static void StoreVarConst();
    static char *GetENV_VAR_CONST(short Index);
    static char **GetPointer_ENV_VAR_CONST();
    static bool isTimeout(const time_t &startInS, time_t Timeout);
    static int changeFileToNonBlocking(int fd, bool closeOnExec = true);

    template<typename T>
    static bool ConvertStrToNum(const char *arr, T &num, short base = 10)
    {
        char *end;
        num = strtol(arr, &end, base);
        if (end[0] != '\0' && !Iswhaitspace(end[0]) && end[0] != '\r' && end[0] != '\n')
            return false;
        return true;
    }
private:
    static std::map<std::string, std::string> _TypeContent;
    static std::map<int, std::string> _Message;
    static std::map<int, std::string> _Body;
    static char _PoinerType[50];
    static char **_ENV_VAR_CONST;
    static bool _Flag;
    HelperFunctions() {}
};


#endif