/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:45 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/08 10:53:07 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsParseOutCGI.hpp"

clsParseOutCGI::clsParseOutCGI()
{
    _BytesBody = 0;
    _FoundBody = false;
    _ProcessIsFinish = false;
    _MaxSizeHeaders = 0;
    _Fdout = -1;
    _Body.resize(40000);
    _Body.clear();
    _HeadersFieldDuplicate.resize(40000);
    _HeadersFieldDuplicate.clear();
    _CounterCGI_Field = 0;
    _RemaindData.resize(4000);
    _RemaindData.clear();
    _Line.resize(4000);
    _Line.clear();
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
            _HeadersFieldDuplicate += Str;
            _HeadersFieldDuplicate += "\r\n";
        }
        else if (Skeep != (int)Value.length() && !Value.empty())
        {
            _HeadersField[Name] += ",";
            _HeadersField[Name] += Value;
        }
    }
    else if (Skeep != (int)Value.length() && !Value.empty())
        _HeadersField[Name] = Value;
    return true;
}

bool clsParseOutCGI::ValidHeaders(std::string &Str)
{
    std::string Name = "", Value;
    size_t Pos = 0;
    if ((Pos = Str.find("\r\n")) != std::string::npos)
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
    if (Name == "location" || Name == "status" || Name == "content-type" )
        _CounterCGI_Field++;
    if (!StoredHeadersField(Name, Value, Str))
        return false;
    return true;
}

void clsParseOutCGI::Connection(bool Isclose)
{
    if (Isclose)
        _HeadersFieldFinal += "Connection: keep-alive";
    else
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
        if (HEAD->first != "status"
                && HEAD->first != "date" && HEAD->first != "server"
                && HEAD->first != "content-length")
        {
            _HeadersFieldFinal += HEAD->first ;
            _HeadersFieldFinal +=":" ;
            _HeadersFieldFinal += HEAD->second ;
            _HeadersFieldFinal += "\r\n";
        }
    }
    _HeadersFieldFinal += _HeadersFieldDuplicate;
    if (_Mod[stMod::CHUNK] == stMod::CHUNK)
        Transfer_Encoding();
    if (_Mod[stMod::CHUNK] != stMod::CHUNK && _BytesBody)
        ContentLength();
    Date();
    Server();
    Connection(true);
    _HeadersFieldFinal += "\r\n";
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
            ErrorRespnseHandling();
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
        ErrorRespnseHandling();
    }
}

void clsParseOutCGI::ReceivingHeaders(std::string &Data)
{
    if (_FoundBody)
        return ;
    bool Flag = false;
    HelperFunctions::GetCleanLineHeader(Data, _Line, &_MaxSizeHeaders, &Flag);
    if (_MaxSizeHeaders > 4000)
    {
        _Status = 413;
        _Mod[stMod::ERROR] = stMod::ERROR;
        return ;
    }
    while(Flag)
    {
        Flag = false;
        if (_MaxSizeHeaders > 4000)
        {
            _Status = 413;
            _Mod[stMod::ERROR] = stMod::ERROR;
            return ;
        }
        if (_Line != "\r\n")
        {
            if (!ValidHeaders(_Line))
            {
                _Status = 502;
                _Mod[stMod::ERROR] = stMod::ERROR;
                return ;
            }
        }
        else
        {
            _FoundBody = true;
            return;
        }
        _Line.clear();
        HelperFunctions::GetCleanLineHeader(Data, _Line, &_MaxSizeHeaders, &Flag);
    }
}

void clsParseOutCGI::ReceivingBody(std::string &Data)
{
    if (_FoundBody && _BytesBody < 40000)
    {
        if (!_CounterCGI_Field)
        {
            _Status = 502;
            _Mod[stMod::ERROR] = stMod::ERROR;
            return ;
        }
       _BytesBody += Data.size();
       if (!_HeadersField.count("content-type") && _BytesBody)
        {
            _Status = 502;
            _Mod[stMod::ERROR] = stMod::ERROR;
            return ;
        }
       if (_BytesBody > 40000)
       {
            _Mod[stMod::CHUNK] = stMod::CHUNK;
            _NameFileBody = "fileout.txt";
            if ((_Fdout = open(_NameFileBody.c_str(),O_CREAT | O_WRONLY, 0644)) == -1)
            {
                std::cout << "---> hohowa hohowa line 298 / calss CGIOUtput <----"<< std::endl;
                _Status = 502;
                _Mod[stMod::ERROR] = stMod::ERROR;
                return ;
            }
            write(_Fdout, &_Body[0], _Body.size());
            write(_Fdout, &Data[0], Data.size());
            Data.clear();
            _Body.clear();
       }
       else
       {
           _Body += Data;
           Data.clear();
       }
    }
    else if (_BytesBody > 40000)
    {
        write(_Fdout, &Data[0], Data.size());
        Data.clear();
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
    _Body.clear();
    _HeadersField.clear();
    _HeadersFieldDuplicate.clear();
    _HeadersFieldFinal.clear();
    close(_Pipe_Fd);
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
    if (_Mod[stMod::ERROR] == stMod::ERROR)
        return ;
    ReceivingHeaders(Data);
    ReceivingBody(Data);
    if (_Mod[stMod::ERROR] == stMod::ERROR)
    {
        ErrorRespnseHandling();
        return ;
    }
    // if not complete so skeep to finishing processe
    if (!_ProcessIsFinish)
        return ;
    else if (!_FoundBody)
    {
        _Status = 502;
        _Mod[stMod::ERROR] = stMod::ERROR;
        ErrorRespnseHandling();
        return ;
    }
    if (_HeadersField.count("location"))
    {
        _Mod[stMod::REDIRECTION] = stMod::REDIRECTION;
        BuilResponsedredirection();
    }
    else
        HeaderResponseCGI();
    _RemaindData.clear();
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
void clsParseOutCGI::SetProcessIsFinish(bool ProcessIsFinish)
{
    _ProcessIsFinish = ProcessIsFinish;
}

const std::string &clsParseOutCGI::GetBody()
{
    return _Body;
}

const std::string &clsParseOutCGI::GetFileNameBody()
{
    return _NameFileBody;
}
clsParseOutCGI::~clsParseOutCGI(){}