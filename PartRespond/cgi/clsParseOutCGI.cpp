/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:45 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/02 17:04:40 by achamdao         ###   ########.fr       */
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
        if (HeaderValue[i] == '\r' || HeaderValue[i] == '\0')
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
    HelperFunctions::Split(DataStaus, StatusLineValue, ' ', -1);
    if (DataStaus.empty())
        return (false);
    if (!HelperFunctions::IsStringDigit(DataStaus[0]))
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
    Value = HelperFunctions::TrimStr(Str.substr(Pos + 1, Str.length()), " \t");

    if (!CheckValidNameHeader(Key))
        return (false);

    Key = HelperFunctions::ConvertStringToLower(Key);
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
        if (!HelperFunctions::IsStringDigit(Value))
            return (false);
        
    Data.push_back(Key);
    Data.push_back(Value);
    return true;
}

bool clsParseOutCGI::StoredCleanHeaders(std::string &Str)
{
    std::vector<std::string> Data(2);
    size_t Pos;
    if ((Pos = HelperFunctions::FindCRLF(Str, "\r\n")) != std::string::npos)
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
        _HeadersFieldFinal += "Connection: keep-alive";
    _HeadersFieldFinal += "Connection: Close";
}
void clsParseOutCGI::StatusNormal()
{
    if (_HeadersField.count("status"))
    {
        _HeadersFieldFinal +=  "HTTP/1.1 ";
        _HeadersFieldFinal +=_HeadersField["status"];
        _HeadersFieldFinal += "\r\n";}
    else
        _HeadersFieldFinal+=  "HTTP/1.1 200 OK\r\n"; 
}
void clsParseOutCGI::StatusRedirection()
{
    if (_HeadersField.count("status"))
    {
        _HeadersFieldFinal +=  "HTTP/1.1 ";
        _HeadersFieldFinal +=_HeadersField["status"];
        _HeadersFieldFinal += "\r\n";
    } 
    else
    {
        _HeadersFieldFinal += "HTTP/1.1 ";
        _HeadersFieldFinal += "302 "; 
        _HeadersFieldFinal += _ErrorPage.GetStatusMessage(302);
        _HeadersFieldFinal +="\r\n";
    }
}

void clsParseOutCGI::HeaderResponseCGI()
{
    std::map <std::string, std::string>::iterator HEAD;

    if (_Mod[stMod::REDIRECTION] != stMod::REDIRECTION)
        StatusNormal();
    else
        StatusRedirection();
    for(HEAD = _HeadersField.begin(); HEAD != _HeadersField.end(); HEAD++)
    {
        if (HEAD->first != "status" && HEAD->first != "location"
                && HEAD->first != "date" && HEAD->first != "server" 
                && HEAD->first != "content-length")
        {
            _HeadersFieldFinal += HEAD->first ;
            _HeadersFieldFinal +=":" ;
            _HeadersFieldFinal += HEAD->second ; 
            _HeadersFieldFinal += "\r\n";
        }
    }
    if (_Mod[stMod::CHUNK] == stMod::CHUNK)
        Transfer_Encoding();
    if (_Mod[stMod::CHUNK] != stMod::CHUNK && _BytesBody)
        ContentLength();
        
    Date();
    Server();
}

void clsParseOutCGI::BuilResponsedredirection()
{
    short CountValidHeader = 0;
    if (_HeadersField.count("content-type"))
        CountValidHeader++;
    if (_HeadersField.count("status"))
        CountValidHeader++;
    if (_BytesBody > 0)
        CountValidHeader++;
    if (LocationIsClientOrLocal(_HeadersField["location"]) && CountValidHeader == 3)
        HeaderResponseCGI();
    else if (!CountValidHeader && LocationIsClientOrLocal(_HeadersField["location"]))
        HeaderResponseCGI();
    else if(!CountValidHeader)
    {
        //geting phisical path from config
        //access file in config
        //read file in config
    }
    else
    {
        //geting page error from RequestHandler from config
        _ErrorPage.SetType("text/html");
        _Body = _ErrorPage.GetBody(500);
        _HeadersFieldFinal = _ErrorPage.ResponseError(500);
    }
}




void clsParseOutCGI::ReceivingData(std::string &Data)
{
    bool InBody = false;
    _RemaindData = Data;
    _Body.resize(2000);
    std::string Line;
    HelperFunctions::GetCleanLine(_RemaindData, Line);
    while(!Line.empty())
    {
        if (Line != "\r\n" && !InBody)
        {
            if (!StoredCleanHeaders(Data))
            {
                _Status = 500;
                _Mod[stMod::ERROR] = stMod::ERROR;
                close(_Pipe_Fd);
                return ;
            }
        }
        else
        {
            _BytesBody  += Data.size();
            InBody = true;
            if (_BytesBody > 2000)
            {
                _Body.clear();
                _Mod[stMod::CHUNK] = stMod::CHUNK;
                // open file and write all data after this if in file
                break;
            }
            _Body += Line;
        }
        HelperFunctions::GetCleanLine(_RemaindData, Line);
    }
    // add other while for stored file
    if (_HeadersField.count("location"))
    {
        _Mod[stMod::REDIRECTION] = stMod::REDIRECTION;
        BuilResponsedredirection();
    }
    else
        HeaderResponseCGI();
}

void clsParseOutCGI::Transfer_Encoding()
{
    _HeadersFieldFinal += "Transfer-Encoding: chunked\r\n";
}


void clsParseOutCGI::Date()
{
    _HeadersFieldFinal += "Date: ";
    _HeadersFieldFinal += HelperFunctions::DateTime();
    _HeadersFieldFinal += "\r\n";
}
void clsParseOutCGI::CachControl()
{
    _HeadersFieldFinal +=  "Cache-Control: no-store\r\n";
}

void clsParseOutCGI::Server()
{
    _HeadersFieldFinal += "Server: HTTP/1.1\r\n";
}

void clsParseOutCGI::ContentLength()
{
    char *Number = HelperFunctions::ft_itoa(_BytesBody);
    _HeadersFieldFinal += "Content-Length: ";
    _HeadersFieldFinal += Number ;
    _HeadersFieldFinal +="\r\n";
    delete[] Number;
}

const std::string &clsParseOutCGI::GetHeadersFieldFinal() const
{
    return _HeadersFieldFinal;
}

void clsParseOutCGI::SetPipe_Fd(int Pipe_Fd)
{
    _Pipe_Fd = Pipe_Fd;
}

clsParseOutCGI::~clsParseOutCGI(){}