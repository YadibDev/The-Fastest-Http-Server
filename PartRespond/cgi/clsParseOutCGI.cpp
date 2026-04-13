/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:45 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/13 10:58:05 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsParseOutCGI.hpp"

clsParseOutCGI::clsParseOutCGI()
{
    _BytesBody = 0;
    _FoundBody = false;
    _ProcessIsFinish = false;
    _MaxSizeHeaders = 4000;
    _CountSizeHeaders = 0;
    _MaxSizeBody = 40000;
    _CountFileTemp = 0;
    _Fdout = -1;
    _CounterCGI_Field = 0;

    _Body.resize(_MaxSizeBody);
    _Body.clear();
    _HeadersFieldDuplicate.resize(_MaxSizeHeaders);
    _HeadersFieldDuplicate.clear();
    _Line.resize(_MaxSizeHeaders);
    _Line.clear();
    _NameHeader.resize(_MaxSizeHeaders);
    _NameHeader.clear();
    _ValueHeader.resize(_MaxSizeHeaders);
    _ValueHeader.clear();

    HelperFunctions::ft_memset(_ExistHeaders, -1, 1);
    HelperFunctions::ft_memset(_Mod, -1, 10);
}

bool clsParseOutCGI::_IsSpecialChar(char C)
{
    return (C == '!'|| C == '#' || C == '$' || C == '%' || C == '&'
            || C == '\'' || C == '*' || C == '+' || C == '-' || C == '.'
            || C == '^' || C == '_' || C == '|' || C == '~' || C == '`');
}

bool clsParseOutCGI::_IsValidHeaderValueChar(unsigned char C)
{
    return (C == '\t' || (C >= ' ' && C <= 126) || (C >= 128 && C <= 255));
}

bool clsParseOutCGI::_CheckValidNameHeader(std::string &HeaderName, short Start, short End)
{
    if (HeaderName.empty())
        return (false);
    short i = Start;
    while (i < (short)HeaderName.length() && i < End)
    {
        if (!isalpha(HeaderName[i]) && !_IsSpecialChar(HeaderName[i])
            && !isdigit(HeaderName[i]))
            return (false);
        i++;
    }
    return (true);
}

bool clsParseOutCGI::_CheckValidValueHeader(std::string &HeaderValue, short Satrt, short End)
{
    short i = Satrt;
    while (i < (short)HeaderValue.length() && i < End)
    {
        if (!_IsValidHeaderValueChar(HeaderValue[i]))
            return (false);
        i++;
    }
    return (true);
}

bool clsParseOutCGI::_LocationIsClientOrLocal(std::string &Location)
{
    if (Location.find("://") != std::string::npos)
        return (true);
    return (false);
}

bool clsParseOutCGI::_ParseStatus(const std::string &LineValue)
{
    short Start = HelperFunctions::SkeeSep(LineValue, " \t");
    short LengthWord = HelperFunctions::LengthWord(LineValue, " \t",Start);
    short End = Start + LengthWord;

    int Counter = HelperFunctions::Countword(LineValue, " \t");
    if (!Counter)
        return false;
    if (!HelperFunctions::IsStringDigit(LineValue, Start, End))
        return (false);
    if (LengthWord != 3)
        return (false);
    return (true);
}

bool clsParseOutCGI::_StoredHeadersField(std::string &Str)
{
    int Skeep = HelperFunctions::SkeeSep(_ValueHeader, " \t");
    if (_HeadersField.count(_NameHeader))
    {
        if (!_NameHeader.compare("location") || !_NameHeader.compare("status") || !_NameHeader.compare("content-type"))
            return false;
        else if (_NameHeader == "set-cookie")
        {
            _HeadersFieldDuplicate += Str;
            _HeadersFieldDuplicate += "\r\n";
        }
        else if (Skeep != (int)_ValueHeader.length() && Skeep)
        {
            _HeadersField[_NameHeader] += ",";                
            _HeadersField[_NameHeader] += _ValueHeader;
            
        }
    }
    else if (Skeep != (int)_ValueHeader.length() && Skeep)
    {
        if (!_NameHeader.compare("content-type"))
            _ExistHeaders[stHeadersCGI::CONTENT_TYPE] = stHeadersCGI::CONTENT_TYPE;
        _HeadersField[_NameHeader] = _ValueHeader;
    }
    _NameHeader.clear();
    _ValueHeader.clear();
    return true;
}

bool clsParseOutCGI::_ValidHeaders(std::string &Str)
{
    size_t Pos = 0;
    if ((Pos = Str.find("\r\n")) != std::string::npos)
        Str.erase(Pos);
    else
        return (false);
    if ((Pos = Str.find(':')) == std::string::npos)
        return (false);    
    if (!_CheckValidNameHeader(Str, 0, Pos))
        return (false);
    if (!_CheckValidValueHeader(Str, Pos + 1, Str.length()))
        return (false);
    
    HelperFunctions::ConvertStringToLower(Str, Pos);
    HelperFunctions::CopyStr(Str,_NameHeader, 0, Pos);
    HelperFunctions::CopyStr(Str,_ValueHeader, Pos + 1, Str.length());
    if (!_NameHeader.compare("status"))
        if (!_ParseStatus(_ValueHeader))
            return false;
    if (!_NameHeader.compare("location") || !_NameHeader.compare("status") || !_NameHeader.compare("content-type"))
        _CounterCGI_Field++;
    else if (!_NameHeader.compare("date") || !_NameHeader.compare("server")
        || !_NameHeader.compare("connection") || !_NameHeader.compare("transfer-encoding"))
    {
        _NameHeader.clear();
        _ValueHeader.clear();
        return true;
    }
    if (!_StoredHeadersField(Str))
        return false;
    return true;
}

void clsParseOutCGI::_Connection(bool Isclose)
{
    if (Isclose)
        _HeadersFieldFinal += "Connection: keep-alive";
    else
        _HeadersFieldFinal += "Connection: Close";
}

void clsParseOutCGI::_StatusNormal()
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

void clsParseOutCGI::_StatusRedirection()
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

void clsParseOutCGI::_HeaderResponseCGI()
{
    std::map <std::string, std::string>::iterator HEAD;
    if (_Mod[stMod::REDIRECTION] != stMod::REDIRECTION)
        _StatusNormal();
    else
        _StatusRedirection();
    for(HEAD = _HeadersField.begin(); HEAD != _HeadersField.end(); HEAD++)
    {
        if (HEAD->first.compare("status") != 0)
        {
            _HeadersFieldFinal += HEAD->first ;
            _HeadersFieldFinal +=":" ;
            _HeadersFieldFinal += HEAD->second ;
            _HeadersFieldFinal += "\r\n";
        }
    }
    _HeadersFieldFinal += _HeadersFieldDuplicate;
    if (_Mod[stMod::CHUNK] == stMod::CHUNK)
        _Transfer_Encoding();
    if (_Mod[stMod::CHUNK] != stMod::CHUNK && _BytesBody)
        _ContentLength();
    _Date();
    _Server();
    _Connection(true);
    _HeadersFieldFinal += "\r\n";
}

void clsParseOutCGI::_BuilResponsedredirection()
{
    uint8_t CountValidHeader = 0;
    if (_HeadersField.count("content-type"))
        CountValidHeader++;
    if (_HeadersField.count("status"))
        CountValidHeader++;
    if (_LocationIsClientOrLocal(_HeadersField["location"]) && CountValidHeader == 2 && _BytesBody)
        _HeaderResponseCGI();
    else if (!CountValidHeader && _LocationIsClientOrLocal(_HeadersField["location"]) && !_BytesBody)
        _HeaderResponseCGI();
    else if(_HeadersField.size() == 1 && !_BytesBody)
    {
        if (access(NULL, F_OK) == -1)
        {
            _Mod[stMod::ERROR] = stMod::ERROR;
            _Status = 404;
            _ErrorRespnseHandling();
            return ;
        }
        _StoredInFileOrStr();
        //geting phisical path from config
        //access file in config
        //read file in config
    }
    else
    {
        //geting page error from RequestHandler from config
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 502;
        _ErrorRespnseHandling();
    }
}

void clsParseOutCGI::_ReceivingHeaders(std::string &Data)
{
    if (_FoundBody)
        return ;
    bool Flag = false;
    HelperFunctions::GetCleanLineHeader(Data, _Line, &_CountSizeHeaders, &Flag);
    if (_CountSizeHeaders > _MaxSizeHeaders)
    {
        _Status = 413;
        _Mod[stMod::ERROR] = stMod::ERROR;
        return ;
    }
    while(Flag)
    {
        Flag = false;
        if (_CountSizeHeaders > _MaxSizeHeaders)
        {
            _Status = 413;
            _Mod[stMod::ERROR] = stMod::ERROR;
            return ;
        }
        if (_Line != "\r\n")
        {
            if (!_ValidHeaders(_Line))
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
        HelperFunctions::GetCleanLineHeader(Data, _Line, &_CountSizeHeaders, &Flag);
    }
}

void clsParseOutCGI::_CreatFileTemp()
{
    char Arr[20] =  "/tmp/CGIout.XXXXXX\0";
    char *temp = mktemp(Arr);
    if (!temp)
    {
        _Status = 502;
        _Mod[stMod::ERROR] = stMod::ERROR;
        return ;
    }
     _NameFileBody =  temp;
}

void clsParseOutCGI::_ReceivingBody(std::string &Data)
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
       if (_ExistHeaders[stHeadersCGI::CONTENT_TYPE]  != stHeadersCGI::CONTENT_TYPE && _BytesBody)
        {
            _Status = 502;
            _Mod[stMod::ERROR] = stMod::ERROR;
            return ;
        }
       if (_BytesBody > _MaxSizeBody)
       {
            _Mod[stMod::CHUNK] = stMod::CHUNK;
            _CreatFileTemp();
            if (_Mod[stMod::ERROR] == stMod::ERROR)
                return ;
            if ((_Fdout = open(_NameFileBody.c_str(),O_CREAT | O_WRONLY, 0644)) == -1)
            {
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
    else if (_BytesBody > _MaxSizeBody)
    {
        write(_Fdout, &Data[0], Data.size());
        Data.clear();
    }
}


void clsParseOutCGI::_StoredInFileOrStr()
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
    if (_BytesBody > _MaxSizeBody)
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

void clsParseOutCGI::_ErrorRespnseHandling()
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
    _Body = HelperFunctions::GetBody(_Status);
    _HeadersFieldFinal =  _ErrorPage.ResponseError(_Status);
}

void clsParseOutCGI::ReceivingData(std::string &Data)
{
    if (_Mod[stMod::ERROR] == stMod::ERROR)
        return ;
    _ReceivingHeaders(Data);
    _ReceivingBody(Data);
    if (_Mod[stMod::ERROR] == stMod::ERROR)
    {
        _ErrorRespnseHandling();
        return ;
    }
    // if not complete so skeep to finishing processe
    if (!_ProcessIsFinish)
        return ;
    else if (!_FoundBody)
    {
        _Status = 502;
        _Mod[stMod::ERROR] = stMod::ERROR;
        _ErrorRespnseHandling();
        return ;
    }
    else
        close(_Fdout);
    if (_HeadersField.count("location"))
    {
        _Mod[stMod::REDIRECTION] = stMod::REDIRECTION;
        _BuilResponsedredirection();
    }
    else
        _HeaderResponseCGI();
}

void clsParseOutCGI::_Transfer_Encoding()
{
    _HeadersFieldFinal += "Transfer-Encoding: chunked\r\n";
}

void clsParseOutCGI::_Date()
{
    _HeadersFieldFinal += "Date: ";
    _HeadersFieldFinal += HelperFunctions::DateTime();
    _HeadersFieldFinal += "\r\n";
}
void clsParseOutCGI::_CachControl()
{
    _HeadersFieldFinal +=  "Cache-Control: no-store\r\n";
}

void clsParseOutCGI::_Server()
{
    _HeadersFieldFinal += "Server: HTTP/1.1\r\n";
}

void clsParseOutCGI::_ContentLength()
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