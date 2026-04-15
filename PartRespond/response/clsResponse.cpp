/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yadib <yadib@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:28 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/15 16:29:50 by yadib            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"
#include "../../Utils/HelperFunctions.hpp"

clsResponse::clsResponse(RequestHandler &DataRequest) : _DataRequest(DataRequest)
{
    _Status = 0;
    _BodySize = 0;
    _MaxSizeHeader = 4000;
    _MaxSizeBody = 40000;
    _FileName = "";
    _FileFromDisk = "";
    _Type = "";
    _IsConnection = true;
    _Erno = false;
    _SizeHeaders = 0;
    _HeaderFeild.resize(_MaxSizeHeader);
    _HeaderFeild.clear();
    _Body.resize(_MaxSizeBody);
    _Body.clear();
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, 10);
}

void clsResponse::MakeResponse()
{
    if (_Mod[stMod::ERROR] != stMod::ERROR && _Mod[stMod::REDIRECTION] != stMod::REDIRECTION)
    {
        _FileFromDisk = _DataRequest.getPhysicalPath();
        _Type = HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(_FileFromDisk));
        StoredInFileOrStr();
    }
    if (_Mod[stMod::ERROR] != stMod::ERROR)
        InitialHeaders();
    else
    {
        ErrorRespnseHandling();
        return;
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
    // yadib modifier this part of achraf
    // std::vector<std::string> Headers = _DataRequest.getHeader.("Connection");
    std::vector<std::string> Headers;
    Headers.push_back(std::string(_DataRequest.getHeader().getKnownHeader(HttpTables::H_CONNECTION)->val.Data));

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

void clsResponse::ErrorRespnseHandling()
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
            _ErrorPage.SetType(HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(ErrorPageConf.uri)));
            _HeaderFeild = _ErrorPage.ResponseError(PrevStatus);
            return;
        }
        else
        {
            _ErrorPage.SetBodySize(0);
            _ErrorPage.SetType(HelperFunctions::GetType(".html"));
            _BodySize += HelperFunctions::ft_strlen(HelperFunctions::GetBody(PrevStatus));
            _HeaderFeild += _ErrorPage.ResponseError(PrevStatus).c_str();
            return;
        }
    }
    _ErrorPage.SetType(HelperFunctions::GetType(".html"));
    _Body += HelperFunctions::GetBody(_Status);
    _BodySize = _Body.size();
    _HeaderFeild += _ErrorPage.ResponseError(PrevStatus);
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
    _HeaderFeild += " ";
    _HeaderFeild += HelperFunctions::GetStatusMessage(_Status);
    _HeaderFeild += "\r\n";
    delete[] Number;
}

void clsResponse::ContentLength()
{
    char *Number = HelperFunctions::ft_itoa(_BodySize);
    _HeaderFeild += "Content-Length: ";
    _HeaderFeild += Number;
    _HeaderFeild += "\r\n";
    delete[] Number;
}
void clsResponse::ContentType()
{
    _HeaderFeild += "Content-Type: ";
    _HeaderFeild += _Type;
    _HeaderFeild += " ; charset=UTF-8\r\n";
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
    _HeaderFeild += "Location: ";
    _HeaderFeild += "value location";
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
    _Body.clear();
    if (_FileFromDisk.empty())
        return;
    if (stat(_FileFromDisk.c_str(), &MetaData) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return;
    }
    _BodySize = MetaData.st_size;
    if (_BodySize > 40000)
    {
        _Mod[stMod::CHUNK] = stMod::CHUNK;
        _FileName = _FileFromDisk;
        return;
    }
    int FD = open(_FileFromDisk.c_str(), O_RDONLY, 644);
    if (FD < 0)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return;
    }
    if (read(FD, &_Body[0], 40000) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        close(FD);
        return;
    }
    close(FD);
}

void clsResponse::ChunkData(std::string &NewStr, const std::string &Str, bool lastChunked) const
{
    if (Str == "")
    {
        NewStr += ("0\r\n\r\n");
        return;
    }
    NewStr += HelperFunctions::Convert_Hex("0123456789abcdef", Str.size());
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
    _FileFromDisk = FileFromDisk;
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
    return _HeaderFeild; // i change here from return object of string to real to normal string
}
// void clsResponse::SetRequestHandler()
// {
// }

clsResponse::~clsResponse() {}