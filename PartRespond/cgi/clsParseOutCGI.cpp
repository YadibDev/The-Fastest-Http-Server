/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:45 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/01 17:56:06 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsParseOutCGI.hpp"

clsParseOutCGI::clsParseOutCGI()
{
    _BytesBody = 0;
    _FoundBody = false;
    _MaxSizeHeaders = 0;
    _Body.resize(40000);
    _Body.clear();
    _RemaindData.resize(4000);
    _RemaindData.clear();
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
    return (true);
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
bool clsParseOutCGI::StoredHeadersField(std::string &Name, std::string &Value, std::string &Str)
{
    int Skeep = HelperFunctions::SkeeSep(Value, " \t");
    if (_HeadersField.count(Name))
    {
        if (Name == "location" || Name == "status" || Name == "content-type" )
            return false;
        else if (Name == "set-cookie")
        {
            _MaxSizeHeaders += Str.length();
            if (_MaxSizeHeaders > 4000) 
                return false;
            _HeadersFieldDuplicate.push_back(Str);
        }
        else if (Skeep != (int)Value.length() && !Value.empty())
        {
            _MaxSizeHeaders += Str.length();
            if (_MaxSizeHeaders > 4000) 
                return false;
            _HeadersField[Name] += ",";
            _HeadersField[Name] += Value;
        }
    }
    else if (Skeep != (int)Value.length() && !Value.empty())
    {
        _MaxSizeHeaders += Str.length();
        if (_MaxSizeHeaders > 4000)
            return false;
        _HeadersField[Name] = Value;
    }
    return true;
}

bool clsParseOutCGI::ValidHeaders(std::string &Str)
{
    std::string Name = "", Value;
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
    Name =  Str.substr(0, Pos);
    Value =  Str.substr(Pos + 1, Str.size());
    HelperFunctions::ConvertStringToLower(Name);
    if (!StoredHeadersField(Name, Value, Str))
        return false;
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
    for (size_t i = 0;i < _HeadersFieldDuplicate.size(); i++)
    {
        _HeadersFieldFinal += _HeadersFieldDuplicate[i];
        _HeadersFieldFinal += "\r\n";
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
    if (LocationIsClientOrLocal(_HeadersField["location"]) && CountValidHeader == 2 && _BytesBody)
        HeaderResponseCGI();
    else if (!CountValidHeader && LocationIsClientOrLocal(_HeadersField["location"]) && !_BytesBody)
        HeaderResponseCGI();
    else if(_HeadersField.size() == 1 && !_BytesBody)
    {
        if (access(NULL, F_OK) == -1)
        {
            _Mod[stMod::ERROR] = stMod::ERROR;
            _Status = 404;
            return ;
        }
        StoredInFileOrStr();
        //geting phisical path from config
        //access file in config
        //read file in config
    }
    else
    {
        //geting page error from RequestHandler from config
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 502;
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
                if (_MaxSizeHeaders > 4000)
                    _Status = 508;
                else
                    _Status = 502;
                _Mod[stMod::ERROR] = stMod::ERROR;
                close(_Pipe_Fd);
                return ;
            }
        }
        else
        {
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
            _NameFileBody = "";
            write(-3, &_Body[0], _Body.size());
            write(-3, &_RemaindData[0], _RemaindData.size());
            _RemaindData.clear();
       }
       else
        _Body += _RemaindData;
    }
    else if (_BytesBody > 40000)
    {
        write(-3, &_RemaindData[0], _RemaindData.size());
        _RemaindData.clear();
    }
}

void clsParseOutCGI::StoredInFileOrStr()
{
    struct stat MetaData;
    _Body = "";
    if (stat(_FileNameFromDisk.c_str(), &MetaData) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return ;
    }
    _BytesBody = MetaData.st_size;
    if (_BytesBody > 40000)
    {
        _Mod[stMod::CHUNK] = stMod::CHUNK;
        _NameFileBody = _FileNameFromDisk;
        return ;
    }
    int FD = open(_FileNameFromDisk.c_str(), O_RDONLY, 644);
    if (FD < 0)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return ;
    }
    if (read(FD,&_Body[0],40000) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        close(FD);
        return ;
    }
    close(FD);
}

void clsParseOutCGI::ErrorRespnseHandling()
{
    // stErrorPagedata ErrorPageConf = _DataRequest.getErrorPage(_Status);
    // short PrevStatus = _Status;
    // if (ErrorPageConf.response)
    // {
    //     if (ErrorPageConf.response != -1)
    //         _Status = ErrorPageConf.response;
    //         _FileNameFromDisk = ErrorPageConf.uri;
    //     StoredInFileOrStr();
    //     if (!_Erno)
    //     {
    //         _ErrorPage.SetMod(_Mod);
    //         _ErrorPage.SetBodySize(_BytesBody);
    //         _ErrorPage.SetType(HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(ErrorPageConf.uri)));
    //         _HeadersFieldFinal += _ErrorPage.ResponseError(PrevStatus);
    //         return ;
    //     }
    //     else
    //     {
    //         _ErrorPage.SetBodySize(0);
    //         _ErrorPage.SetType(HelperFunctions::GetType(".html"));
    //         _Body += HelperFunctions::GetBody(PrevStatus);
    //         _HeadersFieldFinal +=  _ErrorPage.ResponseError(PrevStatus);
    //         return ;
    //     }
    // }
    _ErrorPage.SetType(HelperFunctions::GetType(".html"));
    _Body += HelperFunctions::GetBody(_Status);
    _HeadersFieldFinal +=  _ErrorPage.ResponseError(_Status);
}

void clsParseOutCGI::ReceivingData(std::string &Data)
{
    _RemaindData += Data;
    ReceivingHeaders();
    ReceivingBody();
    if (_Mod[stMod::ERROR] == stMod::ERROR)
    {
        _Body.clear();
        close(_Pipe_Fd);
        ErrorRespnseHandling();
        return ;
    }
    // if not complete so skeep to finishing processe
    if (0)
        return ;
    if (!_HeadersField.count("content-type") && _BytesBody)
    {
        _Body.clear();
        _Status = 502;
        _Mod[stMod::ERROR] = stMod::ERROR;
        close(_Pipe_Fd);
        ErrorRespnseHandling();
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

const std::string &clsParseOutCGI::GetHeadersFieldFinal()
{
    return _HeadersFieldFinal;
}

void clsParseOutCGI::SetPipe_Fd(int Pipe_Fd)
{
    _Pipe_Fd = Pipe_Fd;
}
const std::string &clsParseOutCGI::GetBody()
{
    return _Body;
}

const std::string &clsParseOutCGI::GetFaleNameBody()
{
    return _NameFileBody;
}
clsParseOutCGI::~clsParseOutCGI(){}