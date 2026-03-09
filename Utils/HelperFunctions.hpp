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
#include <string>
#include <ctime>
#include <sys/stat.h>

class HelperFunctions
{
    static size_t	_ft_strlen(const char *s);
public:
    static unsigned long long getCurrentTimeInS();
    static std::string trim(const std::string &str);
    static void skipWhitespace(const std::string &str, size_t &pos);
    static bool isCRLF(const std::string &str);
    static long hexToDec(const std::string &hex);
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




    // Achraf
    static bool CmpWord(const std::string &BigStr, const std::string &Word, bool Switch);
    static size_t FindCRLF(const std::string &Str, const std::string &CRLF);
    static bool IsStringDigit(const std::string &StringDigit);
    static bool Iswhaitspace(char C);
    static std::string TrimStr(std::string Str, const std::string &Sep);
    static void HelperFunctions::ConvertStringToLower(std::string &Str);
    static std::string ConvertStringToUpper(std::string &Str);
    static bool Ischar(const std::string &Sep, char C);
    static int SkeeSep(const std::string &Str, const std::string &Sep);
    static int SkeeSep(const std::string &Str, char Sep);
    static void Split(std::vector<std::string> &Strings, std::string Str, char Sep, int TimesSplit);
    static int ReadData(int FD, std::string &Data, ssize_t Size);
    static std::string GetNextLine(int FD, std::string &BigData, ssize_t Size);
    static std::string GTMHTTP(tm *GMT);
    static std::string DateTime();
    static std::string Convert_Hex(const std::string &Str, int Num);
    static char	*ft_strdup(const char *src);
    static void	free_matrex(char ***matrex);
    static void StoredType(std::map<std::string, std::string> &StoredType, const std::string &FileName);
    static std::string GetTypeDataFile(const std::string &Str);
    static void GetCleanLine(std::string &BigData, std::string &CleanLine);
    static char	*ft_itoa(int n);
    static char	*ft_itoa_negative(int n, char *int_char);
    static int	len_int(int nb);
    static void	*ft_memset(void *str, int c, size_t n);
    static const std::string &GetType(const std::string &Type);
    static void StoredDefaultType();
    static void StoredBodys();
    static void StoredMessage();
    static const std::string &GetStatusMessage(int Status);
    static const std::string &GetBody(int Status);
    static void JoinBuffer(char *OldStr, const char *Newstr, int *UpdateLength);
    static bool ComparHead(const std::string &Str1, const std::string &Str2, short Start, short End);

private:
    static std::map<std::string, std::string> _TypeContent;
    static std::map<int, std::string> _Message;
    static std::map<int, std::string> _Body;
    HelperFunctions() {}
};

#endif