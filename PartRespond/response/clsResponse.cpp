/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:28 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/27 15:39:04 by yadib            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"
#include "../../Utils/HelperFunctions.hpp"

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
    _HeaderFeild.resize(8192);
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, 10);
    HelperFunctions::StoredType(_TypeContent, "response/file.type");
    StoredDefaultType();
}

void clsResponse::MakeResponse()
{
    _HeaderFeild = "";
    if (_Mod[stMod::ERROR] != stMod::ERROR&& _Mod[stMod::REDIRECTION] !=stMod::REDIRECTION)
    {
        _FileFromDisk = _DataRequest.getPhysicalPath();
        _Type = GetTypeData(HelperFunctions::GetTypeDataFile(_FileFromDisk));
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
    std::vector<std::string>  Headers = _DataRequest.getHeaderValues("Connection");
    if (!Headers.empty())
    {
        if (Headers[0] == "Close")
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
    _IsConnection = false;
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
            _ErrorPage.SetType(GetTypeData(HelperFunctions::GetTypeDataFile(ErrorPageConf.uri)));
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
    char *Number = HelperFunctions::ft_itoa(_Status);
    _HeaderFeild += "HTTP/1.1 ";
    _HeaderFeild += Number ;
    _HeaderFeild +=  " ";
    _HeaderFeild += _ErrorPage.GetStatusMessage(_Status) ;
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
    if (_FileFromDisk == "")
        return ;
    if (stat(_FileFromDisk.c_str(), &MetaData) == -1)
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
        _FileName = _FileFromDisk;
        return ;
    }                                
    int FD = open(_FileFromDisk.c_str(), O_RDONLY, 644);
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

const std::string clsResponse::GetTypeData(const std::string &Type)
{
    if (_TypeContent.count(Type))
            return  _TypeContent[Type];
    return "application/octet-stream";
}

void GetType(char *Type)
{
    
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
        _TypeContent[".mp4"]  = "video/mp4";
    }
 }
 const std::string &clsResponse::GetBody() const
{
    return _Body;
}

const std::string &clsResponse::GetFileName() const
{
    return _FileName;
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
    _HeaderFeild = "";
    _IsConnection = false;
    HelperFunctions::StoredType(_TypeContent, "response/file.type");
    StoredDefaultType();
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

clsResponse::~clsResponse()
{
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, 10);
    _Status = 0;
    _BodySize = 0;
    _FileName = "";
    _Body = "";
    _Type = "";
}