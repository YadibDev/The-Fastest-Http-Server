/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:28 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/07 21:42:22 by achamdao         ###   ########.fr       */
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
    _Body = NULL;
    _HeaderFeild = NULL;
    _Type = "";
    _IsConnection = true;
    _Erno = false;
    _SizeHeaders = 0;
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, 10);
}

void clsResponse::MakeResponse()
{
    if (_Mod[stMod::ERROR] != stMod::ERROR&& _Mod[stMod::REDIRECTION] !=stMod::REDIRECTION)
    {
        _FileFromDisk = _DataRequest.getPhysicalPath().c_str();
        _Type = HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(_FileFromDisk));
        StoredInFileOrStr();
    }
    if (_Mod[stMod::ERROR] != stMod::ERROR)
        InitialHeaders();
    else
    {
        ErrorRespnseHandling();
        return ;
    }
    HelperFunctions::JoinBuffer(_HeaderFeild,"\r\n",&_SizeHeaders);
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

void clsResponse::ErrorRespnseHandling()
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
            _ErrorPage.SetType(HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(ErrorPageConf.uri)));
            HelperFunctions::JoinBuffer(_HeaderFeild, _ErrorPage.ResponseError(PrevStatus).c_str(),&_SizeHeaders);
            return ;
        }
        else
        {
            _ErrorPage.SetBodySize(0);
            _ErrorPage.SetType(HelperFunctions::GetType(".html"));
            HelperFunctions::JoinBuffer(_Body, HelperFunctions::GetBody(PrevStatus).c_str(),&_BodySize);
            HelperFunctions::JoinBuffer(_HeaderFeild, _ErrorPage.ResponseError(PrevStatus).c_str(),&_SizeHeaders);
            return ;
        }
    }
    _ErrorPage.SetType(HelperFunctions::GetType(".html"));
    HelperFunctions::JoinBuffer(_Body, HelperFunctions::GetBody(_Status).c_str(),&_BodySize);
    HelperFunctions::JoinBuffer(_HeaderFeild, _ErrorPage.ResponseError(PrevStatus).c_str(),&_SizeHeaders);
}

void clsResponse::ConnectionClose()
{
    HelperFunctions::JoinBuffer(_HeaderFeild,"Connection: Close\r\n",&_SizeHeaders);
}

void clsResponse::Status()
{
    char *Number = HelperFunctions::ft_itoa(_Status);
    HelperFunctions::JoinBuffer(_HeaderFeild, "HTTP/1.1 ",&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, Number,&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild,  " ",&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild,  HelperFunctions::GetStatusMessage(_Status).c_str(),&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild,"\r\n",&_SizeHeaders);
    delete[] Number;
}

void clsResponse::ContentLength()
{
    char *Number = HelperFunctions::ft_itoa(_BodySize);
    HelperFunctions::JoinBuffer(_HeaderFeild, "Content-Length: ",&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, Number,&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, "\r\n",&_SizeHeaders);
    delete[] Number;
}
void clsResponse::ContentType()
{
    HelperFunctions::JoinBuffer(_HeaderFeild, "Content-Type: ",&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, _Type.c_str(),&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, " ; charset=UTF-8\r\n",&_SizeHeaders);

}

void clsResponse::ConnectionKeepAlive()
{
    HelperFunctions::JoinBuffer(_HeaderFeild,"Connection: keep-alive\r\n",&_SizeHeaders);
}
void clsResponse::Transfer_Encoding()
{
    HelperFunctions::JoinBuffer(_HeaderFeild,"Transfer-Encoding: chunked\r\n",&_SizeHeaders);
}

void clsResponse::Redirction()
{
    HelperFunctions::JoinBuffer(_HeaderFeild, "Location: ",&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, "value location",&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, "\r\n",&_SizeHeaders);
}
void clsResponse::Date()
{
    HelperFunctions::JoinBuffer(_HeaderFeild, "Date: ",&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, HelperFunctions::DateTime().c_str(),&_SizeHeaders);
    HelperFunctions::JoinBuffer(_HeaderFeild, "\r\n",&_SizeHeaders);
}
void clsResponse::CachControl()
{
    HelperFunctions::JoinBuffer(_HeaderFeild,"Cache-Control: no-store\r\n",&_SizeHeaders);
}

void clsResponse::Server()
{
    HelperFunctions::JoinBuffer(_HeaderFeild,"Server: HTTP/1.1\r\n",&_SizeHeaders);
}

void clsResponse::StoredInFileOrStr()
{
    struct stat MetaData;
    _Body = NULL;
    if (_FileFromDisk.empty())
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
    if (read(FD,_Body,40000) == -1)
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
    return 0;
}

const std::string &clsResponse::GetFileName() const
{
    return 0;
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
    _IsConnection = false;
}

bool clsResponse::GetIsConnection() const
{
    return _IsConnection;
}

const std::string &clsResponse::GetHeaderFeild() const
{
    return std::string(_HeaderFeild);
}
void clsResponse::SetRequestHandler(const RequestHandler &DataRequest)
{
    _DataRequest = DataRequest;
}

clsResponse::~clsResponse(){}