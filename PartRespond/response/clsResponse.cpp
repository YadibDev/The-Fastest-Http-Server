/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:28 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/22 15:31:30 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"
#include "../../Utils/HelperString.hpp"

clsResponse::clsResponse()
{
    _Status = 0;
    _BodySize = 0;
    _FileName = "";
    _FileFromDisk = "";
    _Body = "";
    _Type = "";
    _IsConnection = true;
    _Erno = false;
    StoredType(_TypeContent, "response/file.type");
    StoredDefaultType();
}

void clsResponse::MakeResponse()
{
    _HeaderFeild = "";
    if (!_Mod.count(ERROR))
    {
        _FileFromDisk = _DataRequest.getPhysicalPath();
        _Type = GetTypeData(GetTypeDataFile(_FileFromDisk));
        StoredInFileOrStr();
    }
    if (!_Mod.count(ERROR))
        InitialHeaders();
    if (_Mod.count(ERROR))
        _HeaderFeild = ErrorRespnseHandling();
    _HeaderFeild += "\r\n";
}

void clsResponse::InitialHeaders()
{
    Status();
    if (!_Mod.count(CHUNK))
        ContentLength();
    if (_BodySize)
        ContentType();
    if (_Mod.count(CHUNK))
        Transfer_Encoding();
    if (_Mod.count(REDIRECTION))
        Redirction();
    Date();
    CachControl();
    Server();
    std::vector<std::string>  Headers = _DataRequest.getHeaderValues("connection");
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

const std::string &clsResponse::ErrorRespnseHandling()
{
    stErrorPagedata ErrorPageConf = _DataRequest.getErrorPage(_Status);
    short PrevStatus = _Status;
    if (ErrorPageConf.response)
    {
        if (ErrorPageConf.response != -1)
            _Status = ErrorPageConf.response;
        _FileFromDisk = ErrorPageConf.uri;
        StoredInFileOrStr();
        if (!_Erno)
        {
            _ErrorPage.SetMod(_Mod);
            _ErrorPage.SetBodySize(_BodySize);
            _ErrorPage.SetType(GetTypeData(GetTypeDataFile(ErrorPageConf.uri)));
            return _ErrorPage.ResponseError(_Status);
        }
        else
        {
            _ErrorPage.SetBodySize(0);
            _ErrorPage.SetType(GetTypeData(".html"));
            _Body = _ErrorPage.GetBody(PrevStatus);
            return _ErrorPage.ResponseError(PrevStatus);
        }
    }
    _ErrorPage.SetType(GetTypeData(".html"));
    _Body = _ErrorPage.GetBody(_Status);
    return _ErrorPage.ResponseError(_Status);
}

void clsResponse::ConnectionClose()
{
    _HeaderFeild += "Connection: Close\r\n";
}

void clsResponse::Status()
{
    std::stringstream Headers;
    Headers << "HTTP/1.1 "<< _Status << " " <<  _ErrorPage.GetStatusMessage(_Status) <<"\r\n";
    _HeaderFeild += Headers.str();
}

void clsResponse::ContentLength()
{
    std::stringstream Headers;
    Headers << "Content-Length: "<< _BodySize<<"\r\n";
    _HeaderFeild += Headers.str();
}
void clsResponse::ContentType()
{
    _HeaderFeild += "Content-Type: ";
    _HeaderFeild += _Type;
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
    std::stringstream Headers;
    Headers << "Location: "<<_DataRequest.getReturn().value<<"\r\n";
    _HeaderFeild += Headers.str();
}
void clsResponse::Date()
{
    std::stringstream Headers;
    Headers << "Date: "<< DateTime() <<"\r\n";     
    _HeaderFeild += Headers.str();
}
void clsResponse::CachControl()
{
    std::stringstream Headers;
    Headers << "Cache-Control: no-store\r\n";
    _HeaderFeild += Headers.str();
}

void clsResponse::Server()
{
    std::stringstream Headers;
    Headers << "Server: HTTP/1.1\r\n";
    _HeaderFeild += Headers.str();
}

void clsResponse::StoredInFileOrStr()
{
    std::string Data;
    _Body = "";
    _BodySize = 0;
    if (_FileFromDisk == "")
        return ;                                     
    int FD = open(_FileFromDisk.c_str(), O_RDONLY, 644);
    if (FD < 0)
    {
        _Mod[ERROR] = ERROR;
        _Status = 500;
        _Erno = true;
        return ;
    }
    if (ReadData(FD, Data, 100) == -1)
    {
        _Mod[ERROR] = ERROR;
        _Status = 500;
        _Erno = true;
        return ;
    }
    while(!Data.empty())
    {
        _BodySize += Data.size();
        if (_BodySize > 2000)
        {
            _Mod[CHUNK] = CHUNK;
            _Body.clear();
            _FileName = _FileFromDisk;
            close(FD);
            return ;
        }
        _Body += Data;
       if (ReadData(FD, Data, 100) == -1)
        {
            _BodySize = 0;
            _Mod[ERROR] = ERROR;
            _Status = 500;
            _Erno = true;
            return ;
        }
    }
    close(FD);
}

const std::string &clsResponse::ChunkData(const std::string &Str)
{
    std::string NewStr;

    if (Str == "")
        return ("0\r\n\r\n");
    NewStr += Convert_Hex("0123456789abcdef",Str.size());
    NewStr += "\r\n";
    NewStr += Str;
    NewStr += "\r\n";
    return (NewStr);
}

const std::string &clsResponse::GetTypeData(const std::string &Type)
{
    if (_TypeContent.count(Type))
            return  _TypeContent[Type];
    return "application/octet-stream";
}
void clsResponse::StoredDefaultType()
{
    if (_TypeContent.empty())
    {
        _TypeContent[".html"] = "text/html";
        _TypeContent[".htm"]  = "text/html";
        _TypeContent[".css"]  = "text/css";
        _TypeContent[".js"]   = "text/javascript";
        _TypeContent[".jpg"]  = "image/jpeg";
        _TypeContent[".jpeg"] = "image/jpeg";
        _TypeContent[".png"]  = "image/png";
        _TypeContent[".txt"]  = "text/plain";
    }
 }
 const std::string &clsResponse::GetBody()
{
    return _Body;
}

const std::string &clsResponse::GetFileName()
{
    return _FileName;
}

void clsResponse::SetStatus(short Status)
{
    _Status = Status;
}

void clsResponse::SetMod(short Mod)
{
    _Mod[Mod] = Mod;
}
void clsResponse::SetType(const std::string &Type)
{
    _Type = Type;
}
void clsResponse::SetFileFromDisk(const std::string &FileFromDisk)
{
    _FileFromDisk = FileFromDisk ;
}

void clsResponse::Reset()
{
    _Status = 0;
    _BodySize = 0;
    _FileName = "";
    _FileFromDisk = "";
    _Body = "";
    _Type = "";
    if (!_HeaderFeild.empty())
        _HeaderFeild.clear();
    if (!_Mod.empty())
        _Mod.clear();
    _IsConnection = false;
    StoredType(_TypeContent, "response/file.type");
    StoredDefaultType();
}

bool clsResponse::GetIsConnection() const
{
    return _IsConnection;
}

const std::string &clsResponse::GetHeaderFeild()
{
    return _HeaderFeild;
}
void clsResponse::SetRequestHandler(const RequestHandler &DataRequest)
{
    _DataRequest = DataRequest;
}

clsResponse::~clsResponse()
{
    if (!_HeaderFeild.empty())
        _HeaderFeild.clear();
    if (!_Mod.empty())
        _Mod.clear();
    _Status = 0;
    _BodySize = 0;
    _FileName = "";
    _Body = "";
    _Type = "";
}