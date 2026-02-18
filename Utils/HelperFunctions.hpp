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

class HelperFunctions
{
public:
    static unsigned long long getCurrentTimeInMs();
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

    // Acheraf
    static bool CmpWord(const std::string &BigStr, const std::string &Word, bool Switch);
    static size_t FindCRLF(const std::string &Str, const std::string &CRLF);
    static bool IsStringDigit(const std::string &StringDigit);
    static bool Iswhaitspace(char C);
    static std::string TrimStr(std::string Str, const std::string &Sep);
    static std::string ConvertStringToLower(std::string &Str);
    static bool Ischar(const std::string &Sep, char C);
    static int SkeeSep(const std::string &Str, const std::string &Sep);
    static int SkeeSep(const std::string &Str, char Sep);
    static std::vector<std::string> Split(std::string Str, char Sep, int TimesSplit);
    static int ReadData(int FD, std::string &Data, ssize_t Size);
    static std::string GetNextLine(int FD, std::string &BigData, ssize_t Size);
    static std::string GTMHTTP(tm *GMT);
    static std::string DateTime();
    static std::string Convert_Hex(const std::string &Str, int Num);

private:
    HelperFunctions() {}
};

#endif