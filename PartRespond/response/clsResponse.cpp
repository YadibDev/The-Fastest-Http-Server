/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:28 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/17 15:07:10 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"

clsResponse::clsResponse()
{
    _Status = 0;
    _BodySize = 0;
    _FileName = "";
    _FileFromDisk = "";
    _Body = "";
    _Type = "";
    _IsConnection = false;
    StoredType(_TypeContent, "response/file.type");
    StoredDefaultType();
}

std::string clsResponse::MakeResponse()
{
    if (!_Mod.count(ERROR))
        StoredInFileOrStr();
    if (!_Mod.count(ERROR))
        InitialHeaders();
    if (_Mod.count(ERROR))
        return ErrorRespnseHandling();
     _HeaderFeild += "\r\n";
    return _HeaderFeild;
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
    // if the client want close conection we called function Close connection
    ConnectionKeepAlive();
}

std::string clsResponse::ErrorRespnseHandling()
{
    // search in class request handler for 
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
    std::stringstream Headers;
    Headers << "Transfer-Encoding: chunked\r\n";
    _HeaderFeild += Headers.str();
}

void clsResponse::Redirction()
{
    std::stringstream Headers;
    // get data from config file
    Headers << "Location: "<<"..."<<"\r\n";
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
    Headers << "Server: Name Server\r\n";
    _HeaderFeild += Headers.str();
}

void clsResponse::StoredInFileOrStr()
{
    std::string Data;
    if (_FileFromDisk == "")
        return ;                                         
    int FD = open(_FileFromDisk.c_str(), O_RDONLY, 644);
    if (FD < 0)
    {
        _Mod[ERROR] = ERROR;
        _Status = 500;
        return ;
    }
    ReadData(FD, Data, 100);
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
        ReadData(FD, Data, 100);
    }
}

std::string clsResponse::ChunkData(const std::string &Str)
{
    std::string NewStr;

    if (Str == "")
        return ("0\r\n");
    NewStr += Convert_Hex("0123456789abcdef",Str.size());
    NewStr += "\r\n";
    NewStr += Str;
    NewStr += "\r\n";
    return (NewStr);
}

std::string clsResponse::GetTypeData(std::string Type)
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
std::string clsResponse::GetBody()
{
    return _Body;
}

std::string clsResponse::GetFileName()
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
void clsResponse::SetType(std::string Type)
{
    _Type = Type;
}
void clsResponse::SetFileFromDisk(std::string FileFromDisk)
{
    _FileFromDisk = FileFromDisk ;
}
clsResponse::~clsResponse()
{
    _HeaderFeild.clear();
    _Mod.clear();
    _Status = 0;
    _BodySize = 0;
    _FileName = "";
    _Body = "";
    _Type = "";
}