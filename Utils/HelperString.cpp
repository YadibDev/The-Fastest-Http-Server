/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperString.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 20:47:16 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/12 14:53:22 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HelperString.hpp"

bool CmpWord (const std::string &BigStr, const std::string &Word, bool Switch)
{
    int i = 0;
    int lenghtBigStr = BigStr.size();
    int lenghtWord = Word.size();

    if (Switch)
    {
        while (i < lenghtBigStr && i < lenghtWord)
        {
            if (BigStr[i] != Word[i])
                return (false);
            i++;
        }
    }
    else
    {
        while (lenghtBigStr && lenghtWord)
            if (BigStr[lenghtBigStr--] != Word[lenghtWord--])
                return (false);
    }
    if (!lenghtWord || lenghtWord == i)
        return (true);
    return (false);
}

size_t FindCRLF(const std::string &Str, const std::string &CRLF)
{
    size_t i = (Str.size())? Str.size() - 1 : 0;
    size_t j = (CRLF.size())? CRLF.size() - 1 : 0;
    if (!i)
        return (std::string::npos);
    while (true)
    {
        if (!Iswhaitspace(Str[i]))
            if (CRLF[j] != Str[i])
                break;
        if (!i || !j)
            break;
        if (!Iswhaitspace(Str[i]))
            j--;
        i--;
    }
    if (!j)
        return (i);
    return (std::string::npos);
}

bool IsStringDigit(const std::string &StringDigit)
{
    int i = 0;
    if (StringDigit.empty())
        return false;
    while (i < StringDigit.size())
    {
        if (!std::isdigit(StringDigit[i]))
            return (false);
        i++;
    }
    return (true);
}

bool Iswhaitspace(char C)
{
    return (C == ' ' || C == '\t');
}

std::string  TrimStr(std::string Str,const std::string &Sep)
{
    int Start = 0;
    int End = Str.length() - 1;
    if (Str.empty())
        return (Str);
    while (Ischar(Sep, Str[Start]))        
        Start++;
    while (End > Start && Ischar(Sep, Str[End]))
        End--;
    return (Str.substr(Start, End - Start + 1));
}

std::string ConvertStringToLower(std::string &Str)
{
    int i = 0;
    while (i < Str.size())
    {
        if (std::isalpha(Str[i]))
            Str[i] = std::tolower(Str[i]);
        i++;
    }
    return Str;
}

bool Ischar(const std::string &Sep, char C)
{
    int i = 0;
    while (i < Sep.size())
        if (Sep[i++] == C)
            return (true);
    return (false);
}

int SkeeSep(const std::string &Str, const std::string &Sep)
{
    int i = 0;
    while (Ischar(Sep, Str[i]))
        i++;
    return (i);
}

int SkeeSep(const std::string &Str, char Sep)
{
    int i = 0;
    while (i < Str.size() && Sep == Str[i])
        i++;
    return (i);
}

std::vector<std::string> Split(std::string Str, char Sep , int TimesSplit)
{
    std::vector<std::string> Strings;
    unsigned long Pos = 0;
    std::string  SepString = "";
    int i = 0;
    if (Str.empty())
        return (Strings);
    if ((Pos = Str.find(Sep)) == std::string::npos)
    {
        Strings.push_back(Str);
        return Strings;
    }
    else
    {
        Str = Str.substr(SkeeSep(Str, Sep), Str.length());
        while ((Pos = Str.find(Sep)) != std::string::npos)
        {
            if (i == TimesSplit - 1 && TimesSplit > 0)
                break;
            else if (TimesSplit > 0)
                i++;
            Strings.push_back(Str.substr(0,Pos));
            Pos += SkeeSep(Str.substr(Pos , Str.length()),Sep);
            Str = Str.substr(Pos , Str.length());
        }
        if (!Str.empty())
            Strings.push_back(Str);
    }
    return Strings;
}

int ReadData(int FD, std::string &Data, size_t Size)
{
    Data.resize(Size);
    ssize_t SizeByte = read(FD, &Data[0], Size);
    if (SizeByte < 0)
        return -1;
    if (SizeByte != Size)
        Data.resize(SizeByte);
    return (SizeByte);
}

std::string GetNextLine(int FD, std::string &BigData, size_t Size)
{
    std::string Buffer;
    std::string CleanLine;
    size_t Pos = 0;
    ssize_t SizeByte = 0;

    while ((Pos = BigData.find('\n')) == std::string::npos)
    {
        SizeByte = ReadData(FD, Buffer, Size);
        if (SizeByte < 0)
            return ("");
        if (SizeByte == 0)
            break;
        BigData += Buffer;
    }
    if (Pos != std::string::npos)
    {
        Pos += 1;
        CleanLine = BigData.substr(0, Pos);
        BigData = BigData.substr( Pos, BigData.size() - Pos);
    }
    else
    {
        CleanLine = BigData;
        BigData.clear();
    }
    return CleanLine;
}

std::string GTMHTTP( tm* GMT)
{
    std::string Str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    std::string Months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    std::stringstream StrGTMHTTP;
    StrGTMHTTP << Str[GMT->tm_wday] << ", ";
    StrGTMHTTP << GMT->tm_mday << " ";
    StrGTMHTTP << Months[GMT->tm_mon] << " ";
    StrGTMHTTP << GMT->tm_year + 1900<< " ";
    StrGTMHTTP << GMT->tm_hour << ":"<< GMT->tm_min<< ":"<<GMT->tm_sec;
    StrGTMHTTP << " GMT";
    return StrGTMHTTP.str();
}

std::string DateTime() 
{
    time_t Time = time(0);
    tm* GMT = gmtime(&Time);
    std::string DateTime = GTMHTTP(GMT);
    return DateTime;
}

std::string	Convert_Hex(const std::string &Str, int Num)
{
	int		i = 0;
	std::string	MaxHex;
	std::string	Result;

	if (Num == 0)
		return ("0");
	while (Num > 0)
	{
		MaxHex += Str[Num % 16];
		Num /= 16;
	}
    i = MaxHex.size() - 1;
	while (i >= 0)
		Result += MaxHex[i--];
	return (Result);
}