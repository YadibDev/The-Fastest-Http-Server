/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:45 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/09 22:00:42 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsParseOutCGI.hpp"

clsParseOutCGI::clsParseOutCGI()
{
    _BytesBody = 0;
    _CountHeader = 0;
    _FoundBody = false;
    StoredBlackListHeaders(_BlackListHeaders);
    _Body.resize(40000);
    _RemaindData.resize(4000);
}

bool clsParseOutCGI::IsSpecialChar(char C)
{
    return (C == '!'|| C == '#' || C == '$' || C == '%' || C == '&' 
            || C == '\'' || C == '*' || C == '+' || C == '-' || C == '.' 
            || C == '^' || C == '_' || C == '|' || C == '~' || C == '`');
}

bool clsParseOutCGI::CheckValidNameHeader(std::string &HeaderName, short Start, short End)
{
    if (HeaderName.empty())
        return (false);
    int i = Start;
    while (i < HeaderName.length() && i < End)
    {
        if (!isalpha(HeaderName[i]) && !IsSpecialChar(HeaderName[i])
            && !isdigit(HeaderName[i]))
            return (false);
        i++;
    }
    return (true);
}

bool clsParseOutCGI::CheckValidValueHeader(std::string &HeaderValue, short Satrt, short End)
{
    size_t i = Satrt;
    while (i < HeaderValue.length() && i < End)
    {
        if (HeaderValue[i] == '\r' || HeaderValue[i] == '\0')
            return (false);
        i++;
    }
    if (i == 0)
        return (false);
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

bool clsParseOutCGI::ValidHeaders(std::string &Str)
{
    std::string Key = "";
    size_t Pos = 0;
    if ((Pos = HelperFunctions::FindCRLF(Str, "\r\n")) != std::string::npos)
        Str.erase(Pos);
    else
        return (false);
    if ((Pos = Str.find(':')) == std::string::npos)
        return (false);

    if (!CheckValidNameHeader(Str, 0, Pos))
        return (false);

    if (!CheckValidValueHeader(Str, Pos + 1, Str.length()))
        return (false);
    if (HelperFunctions::ComparHead(Str, "status", 0, Pos))
        if (!ParseStatus(Str.substr(Pos + 1, Str.length())))
            return (false);
    else if (HelperFunctions::ComparHead(Str, "location", 0, Pos))
        if (0);
    Key = Str.substr(0, Pos);
    HelperFunctions::ConvertStringToLower(Key);
    if (!_HeadersField.count(Key))
        _HeadersField[Key] = Str.substr(Pos + 1, Str.length());
    else
        return (false);
    return true;
}

void clsParseOutCGI::Connection(bool Isclose)
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
        _HeadersFieldFinal += "\r\n";
    }
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
        _HeadersFieldFinal += HelperFunctions::GetStatusMessage(302);
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
    else if(!CountValidHeader && !_BytesBody)
    {
        //geting phisical path from config
        //access file in config
        //read file in config
    }
    else
    {
        //geting page error from RequestHandler from config
        _ErrorPage.SetType("text/html");
        _Body = HelperFunctions::GetBody(502);
        _HeadersFieldFinal = _ErrorPage.ResponseError(502);
    }
}

void clsParseOutCGI::ReceivingHeaders()
{
    if (_FoundBody)
        return ;
    std::string Line;
    HelperFunctions::GetCleanLine(_RemaindData, Line);
    while(!Line.empty())
    {
        if (Line != "\r\n")
        {
            if (!ValidHeaders(Line))
            {
                _Status = 502;
                _Mod[stMod::ERROR] = stMod::ERROR;
                close(_Pipe_Fd);
                return ;
            }
        }
        else
        {
            _RemaindData.erase(0, 2);
            _FoundBody = true;
            return;
        }
        HelperFunctions::GetCleanLine(_RemaindData, Line);
    }
}

void clsParseOutCGI::ReceivingBody()
{
    if (_FoundBody && _BytesBody < 40000)
    {
       _BytesBody += _RemaindData.size();
       if (_BytesBody > 40000)
       {
            _Mod[stMod::CHUNK] = stMod::CHUNK;
            // open file for storing
            _FileName = "";
            write(1, &_Body[0], _Body.size());
            write(1, &_RemaindData[0], _RemaindData.size());
            _RemaindData.clear();
       }
       else
        _Body += _RemaindData;
    }
    else
    {
        write(1, &_RemaindData[0], _RemaindData.size());
        _RemaindData.clear();
    }
    
}

void clsParseOutCGI::ReceivingData(std::string &Data)
{
    _RemaindData += Data;

    ReceivingHeaders();
    ReceivingBody();
    // add other while for stored file
    if (!_HeadersField.count("content-type") && _BytesBody)
    {
        _Status = 502;
        _Mod[stMod::ERROR] = stMod::ERROR;
        close(_Pipe_Fd);
    }
    else if (_HeadersField.count("location"))
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