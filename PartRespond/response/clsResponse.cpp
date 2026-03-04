/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:28 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/04 21:49:05 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"
#include "../../Utils/HelperFunctions.hpp"

clsResponse::clsResponse()
{
    _Status = 0;
    _BodySize = 0;
    _FileName = NULL;
    _FileFromDisk = NULL;
    _Body = "";
    _Type = NULL;
    _IsConnection = true;
    _Erno = false;
    _HeaderFeild.resize(8192);
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, 10);
}

void clsResponse::MakeResponse()
{
    _HeaderFeild = "";
    if (_Mod[stMod::ERROR] != stMod::ERROR&& _Mod[stMod::REDIRECTION] !=stMod::REDIRECTION)
    {
        *_FileFromDisk = _DataRequest.getPhysicalPath();
        *_Type = HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(*_FileFromDisk));
        StoredInFileOrStr();
    }
    if (_Mod[stMod::ERROR] != stMod::ERROR)
        InitialHeaders();
    else
    {
        _HeaderFeild = ErrorRespnseHandling();
        return ;
    }
    _HeaderFeild += "\r\n";
}

void clsResponse::InitialHeaders()
{
    Status();
    if (_Mod[stMod::CHUNK] != stMod::CHUNK)
        ContentLength();
    if (_BodySize)
        ContentType();
    if (_Mod[stMod::CHUNK] == stMod::CHUNK)
        Transfer_Encoding();
    if (_Mod[stMod::REDIRECTION] == stMod::REDIRECTION)
        Redirction();
    Date();
    CachControl();
    Server();
    std::vector<std::string>  Headers = _DataRequest.getHeaderValues("connection");

    // this handel by enums and git value from array
    
    if (!Headers.empty())
    {
        if (Headers[0] == "close")
        {
            _IsConnection = false;
            ConnectionClose();
        }
        else
            ConnectionKeepAlive();
    }
    else
        ConnectionKeepAlive();
}

const std::string clsResponse::ErrorRespnseHandling()
{
    stErrorPagedata ErrorPageConf = _DataRequest.getErrorPage(_Status);
    short PrevStatus = _Status;
    if (ErrorPageConf.response)
    {
        if (ErrorPageConf.response != -1)
            _Status = ErrorPageConf.response;
        _FileFromDisk = &ErrorPageConf.uri;
        StoredInFileOrStr();
        if (!_Erno)
        {
            _ErrorPage.SetMod(_Mod);
            _ErrorPage.SetBodySize(_BodySize);
            _ErrorPage.SetType(HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(ErrorPageConf.uri)));
            return _ErrorPage.ResponseError(_Status);
        }
        else
        {
            _ErrorPage.SetBodySize(0);
            _ErrorPage.SetType(HelperFunctions::GetType(".html"));
            _Body = HelperFunctions::GetBody(PrevStatus);
            return _ErrorPage.ResponseError(PrevStatus);
        }
    }
    _ErrorPage.SetType(HelperFunctions::GetType(".html"));
    _Body = HelperFunctions::GetBody(_Status);
    return _ErrorPage.ResponseError(_Status);
}

void clsResponse::ConnectionClose()
{
    _HeaderFeild += "Connection: Close\r\n";
}

void clsResponse::Status()
{
    char *Number = HelperFunctions::ft_itoa(_Status);
    _HeaderFeild += "HTTP/1.1 ";
    _HeaderFeild += Number;
    _HeaderFeild +=  " ";
    _HeaderFeild += HelperFunctions::GetStatusMessage(_Status);
    _HeaderFeild += "\r\n";
    delete[] Number;
}

void clsResponse::ContentLength()
{
    char *Number = HelperFunctions::ft_itoa(_BodySize);
    _HeaderFeild += "Content-Length: ";
    _HeaderFeild += Number ;
    _HeaderFeild +="\r\n";
    delete[] Number;
}
void clsResponse::ContentType()
{
    _HeaderFeild += "Content-Type: ";
    _HeaderFeild += *_Type;
    _HeaderFeild +=" ; charset=UTF-8\r\n";
}

void clsResponse::ConnectionKeepAlive()
{
    _HeaderFeild += "Connection: keep-alive\r\n";
}
void clsResponse::Transfer_Encoding()
{
    _HeaderFeild += "Transfer-Encoding: chunked\r\n";
}

void clsResponse::Redirction()
{
    _HeaderFeild +=  "Location: ";
    _HeaderFeild += _DataRequest.getReturn().value ;
    _HeaderFeild += "\r\n";
}
void clsResponse::Date()
{
    _HeaderFeild += "Date: ";
    _HeaderFeild += HelperFunctions::DateTime();
    _HeaderFeild += "\r\n";
}
void clsResponse::CachControl()
{
    _HeaderFeild += "Cache-Control: no-store\r\n";
}

void clsResponse::Server()
{
    _HeaderFeild += "Server: HTTP/1.1\r\n";
}

void clsResponse::StoredInFileOrStr()
{
    struct stat MetaData;
    _Body = "";
    if (!_FileFromDisk)
        return ;
    if (stat(_FileFromDisk->c_str(), &MetaData) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return ;
    }
    _BodySize = MetaData.st_size;
    if (_BodySize > 40000)
    {
        _Mod[stMod::CHUNK] = stMod::CHUNK;
        _Body.clear();
        _FileName = _FileFromDisk;
        return ;
    }                                
    int FD = open(_FileFromDisk->c_str(), O_RDONLY, 644);
    if (FD < 0)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return ;
    }
    if (HelperFunctions::ReadData(FD, _Body, 40000) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return ;
    }
    close(FD);
}

void clsResponse::ChunkData(std::string &NewStr, const std::string &Str, bool lastChunked) const
{
    if (Str == "")
    {
        NewStr += ("0\r\n\r\n");
        return ;
    }
    NewStr += HelperFunctions::Convert_Hex("0123456789abcdef",Str.size());
    NewStr += "\r\n";
    NewStr += Str;
    NewStr += "\r\n";
    if (lastChunked)
        NewStr += "0\r\n\r\n";
}

 const std::string &clsResponse::GetBody() const
{
    return _Body;
}

const std::string &clsResponse::GetFileName() const
{
    return *_FileName;
}

void clsResponse::SetStatus(short Status)
{
    _Status = Status;
}

void clsResponse::SetMod(stMod::eMod Mod)
{
    _Mod[Mod] = Mod;
}
void clsResponse::SetType(const std::string &Type)
{
    *_Type = Type;
}
void clsResponse::SetFileFromDisk(const std::string &FileFromDisk)
{
    *_FileFromDisk = FileFromDisk ;
}

void clsResponse::Reset()
{
    _Status = 0;
    _BodySize = 0;
    _FileName = NULL;
    _FileFromDisk = NULL;
    _Body = "";
    _Type = NULL;
    _HeaderFeild = "";
    _IsConnection = false;
}

bool clsResponse::GetIsConnection() const
{
    return _IsConnection;
}

const std::string &clsResponse::GetHeaderFeild() const
{
    return _HeaderFeild;
}
void clsResponse::SetRequestHandler(const RequestHandler &DataRequest)
{
    _DataRequest = DataRequest;
}

clsResponse::~clsResponse(){}