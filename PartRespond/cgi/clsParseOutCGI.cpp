/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:45 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/10 17:38:24 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsParseOutCGI.hpp"

clsParseOutCGI::clsParseOutCGI()
{
    _BytesBody = 0;
    _SizeFile = 0;
    _FoundBody = false;
    StoredBlackListHeaders(_BlackListHeaders);
}

bool clsParseOutCGI::IsSpecialChar(char C)
{
    return (C == '!'|| C == '#' || C == '$' || C == '%' || C == '&' 
            || C == '\'' || C == '*' || C == '+' || C == '-' || C == '.' 
            || C == '^' || C == '_' || C == '|' || C == '~' || C == '`');
}

bool clsParseOutCGI::CheckValidNameHeader(std::string &HeaderName)
{
    if (HeaderName.empty())
        return (false);
    int i = 0;
    while (i < HeaderName.length())
    {
        if (!isalpha(HeaderName[i]) && !IsSpecialChar(HeaderName[i])
            && !isdigit(HeaderName[i]))
            return (false);
        i++;
    }
    return (true);
}

bool clsParseOutCGI::CheckValidValueHeader(std::string &HeaderValue)
{
    int i = 0;
    while (i < HeaderValue.length())
    {
        if (HeaderValue[i] == '\r' || HeaderValue[i] == '\n'
            || HeaderValue[i] == '\0')
            return (false);
        i++;
    }
    return (true);
}

void clsParseOutCGI::StoredBlackListHeaders(std::vector <std::string> &BalckListHeader)
{
    BalckListHeader.push_back("content-length");
    BalckListHeader.push_back("transfer-encoding");
    BalckListHeader.push_back("connection");
    BalckListHeader.push_back("x-cgi-");
}

bool clsParseOutCGI::LocationIsClientOrLocal(std::string &Location)
{
    if (Location.find("://") != std::string::npos)
        return (true);
    return (false);
}

bool clsParseOutCGI::ParseStatus(const std::string &StatusLineValue)
{
    std::vector <std::string> DataStaus;
    if (StatusLineValue.empty())
        return (false);
    DataStaus = Split(StatusLineValue, ' ', -1);
    if (DataStaus.empty())
        return (false);
    if (!IsStringDigit(DataStaus[0]))
        return (false);
    if (DataStaus[0].size() != 3)
        return (false);
    return (true);
}

bool clsParseOutCGI::ValidHeaders(std::string &Str, std::vector <std::string> &Data)
{
    std::string Key = "" , Value = "";
    size_t Pos = 0;
    
    if ((Pos = Str.find(':')) == std::string::npos)
        return (false);
    Key = Str.substr(0, Pos);
    Value = TrimStr(Str.substr(Pos + 1, Str.length()), " \t");

    if (!CheckValidNameHeader(Key))
        return (false);

    Key = ConvertStringToLower(Key);
    if (Value.empty())
    {
        if (_SpecialHeaders.empty())
            return (false);
        if (!_SpecialHeaders.count(Key))
            return false;
    }
    if (!CheckValidValueHeader(Value))
        return (false);
    if (Key == "status")
        if (!ParseStatus(Value))
            return (false);
    if (Key == "content-length")
        if (!IsStringDigit(Value))
            return (false);
        
    Data.push_back(Key);
    Data.push_back(Value);
    return true;
}

bool clsParseOutCGI::StoredCleanHeaders(std::string &Str)
{
    std::vector<std::string> Data;
    size_t Pos;
    if ((Pos = FindCRLF(Str, "\r\n\r\n")) != std::string::npos )
    {
        Str = Str.substr(0, Pos);
        _FoundBody = true;
    }
    else if ((Pos = FindCRLF(Str, "\r\n")) != std::string::npos)
        Str = Str.substr(0, Pos);
    else
        return (false);

    if (!ValidHeaders(Str, Data))
        return (false);
    if (!_HeadersField.count(Data[0]))
        _HeadersField[Data[0]] = Data[1];
    else
        return (false);
    return (true);
}

std::string clsParseOutCGI::Connection(bool Isclose)
{
    if (Isclose)
        return ("Connection: keep-alive");
    return ("Connection: Close");
}

std::string clsParseOutCGI::HeaderResponseCGI()
{
    std::stringstream Headers;
    std::map <std::string, std::string>::iterator HEAD;
    if (!_HeadersField.count("location"))
    {
        if (_HeadersField.count("status"))
            Headers << "HTTP/1.1 "<<_HeadersField["status"]<<"\r\n"; 
        else
            Headers << "HTTP/1.1 "<<"200 OK"<<"\r\n"; 
    }
    else
    {
        if (_HeadersField.count("status"))
            Headers << "HTTP/1.1 "<<_HeadersField["status"]<<"\r\n"; 
        else
            Headers << "HTTP/1.1 "<<  "302 "<<ErrorPage.GetStatusMessage(302)<<"\r\n"; 
    }
    for(HEAD = _HeadersField.begin(); HEAD != _HeadersField.end(); HEAD++)
    {
        if (HEAD->first != "status")
            Headers << HEAD->first << ":" <<  HEAD->second << "\r\n";
    }
    Headers << Connection(true)<< "\r\n";
    Headers << "Server: Webserv/1.0\r\n";
    Headers << "\r\n";
    return (Headers.str());
}

std::string clsParseOutCGI::BuilResponsedredirection()
{
    short CountValidHeader = 0;
    if (_HeadersField.count("content-type"))
        CountValidHeader++;
    if (_HeadersField.count("status"))
        CountValidHeader++;
    if (_BytesBody > 0)
        CountValidHeader++;
    if (LocationIsClientOrLocal(_HeadersField["location"]) && CountValidHeader == 3)
        return HeaderResponseCGI();
    else if (!CountValidHeader && LocationIsClientOrLocal(_HeadersField["location"]))
        return HeaderResponseCGI();
    else if(!CountValidHeader)
        ;// problem access file in config
    else
        return ErrorPage.ResponseError(500);
}

void clsParseOutCGI::ReceivingData(std::string &Data)
{
    _SizeFile  += Data.size();
    if (!_FoundBody)
    {
        if (!StoredCleanHeaders(Data))
        {
            _Status = 500;
            _Mod = ERROR;
            return ;
        }
    }
    else
    {
        _BytesBody  += Data.size();
        // stored body in file or string
    }
}
