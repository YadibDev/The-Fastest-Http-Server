/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yadib <yadib@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:28 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/07 14:10:29 by yadib            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"
#include "../../Utils/HelperFunctions.hpp"

clsResponse::clsResponse(RequestHandler &DataRequest) : _DataRequest(DataRequest)
{
    _Status = 0;
    _BodySize = 0;

    _IsConnection = true;
    _SizeHeaders = 0;
    _Erno = false;
    _ModTransferData = false;
    _FileFromDisk.resize(1000);
    _HeaderFeild.resize(MAX_HEADERS);
    _InternalRedirectSrc.resize(1000);
    _Body.resize(MAX_BODY);
    _Type.resize(500);
    if (_InternalRedirectSrc.empty() || _Type.empty() || _HeaderFeild.empty() || _FileFromDisk.empty() || _Body.empty())
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return;
    }
    _FileFromDisk.clear();
    _HeaderFeild.clear();
    _InternalRedirectSrc.clear();
    _Type.clear();
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, sizeof(_Mod));
}

void clsResponse::MakeResponse()
{
    if (_Erno)
        return;
    if (_Mod[stMod::GET] == stMod::GET)
    {
        if (!_DataRequest.getAutoIndex())
        {
            _FileFromDisk = _DataRequest.getPhysicalPath();
            _Type = HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(_FileFromDisk));
            _StoredInFileOrStr();
        }
        else
            _InitialAutoIndex();
    }
    if (_Mod[stMod::ERROR] != stMod::ERROR)
        _InitialHeaders();
    if (_Mod[stMod::ERROR] == stMod::ERROR)
    {
        _ErrorRespnseHandling();
        return;
    }
}

void clsResponse::_InitialAutoIndex()
{
    _Type = HelperFunctions::GetType(".html");
    _Mod[stMod::CHUNK] = stMod::CHUNK;
    _Mod[stMod::AUTOINDEX] = stMod::AUTOINDEX;
    if (AutoIndex.initializeAutoIndex(_DataRequest.getPhysicalPath(),
                                      _DataRequest.getRequestUri().Data, HelperFunctions::ft_strlen(_DataRequest.getPhysicalPath()),
                                      _DataRequest.getRequestUri().len) == ERROR_AUTO_INDEX)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = AutoIndex.getErrorCode();
    }
}

void clsResponse::_InitialHeaders()
{
    _StatusLine();
    if (_Mod[stMod::CHUNK] != stMod::CHUNK && _Mod[stMod::DELETE] != stMod::DELETE)
        _ContentLength();
    if (_Mod[stMod::REDIRECTION] == stMod::REDIRECTION)
    {
        if (_Status == 301 || _Status == 302 || _Status == 303 || _Status == 307 || _Status == 308)
            _Redirction();
        else
        {
            _Body = _DataRequest.getReturn().value.raw_path;
            _BodySize = _Body.size();
        }
    }
    if (!_Type.empty())
        _ContentType();
    if (_Mod[stMod::CHUNK] == stMod::CHUNK)
        _Transfer_Encoding();
    if (_DataRequest.getHeader().getKnownHeader(HttpTables::H_CONNECTION)->Hash != -1)
    {
        if (HelperFunctions::CmpWord(_DataRequest.getHeader().getKnownHeader(HttpTables::H_CONNECTION)->val.Data,
                                     "close", _DataRequest.getHeader().getKnownHeader(HttpTables::H_CONNECTION)->val.len))
        {
            _IsConnection = false;
            _Connection(false);
        }
        else
            _Connection(true);
    }
    else
        _Connection(true);
    _Date();
    _Server();
    _HeaderFeild += "\r\n";
}

void clsResponse::_ErrorRespnseHandling()
{
    if (_Mod[stMod::INTERNALRE] != stMod::INTERNALRE)
        return;
    else
    {
        _Mod[stMod::ERROR] = stMod::EMPTY;
        if (!_DataRequest.getDefaultErrorPage())
        {
            if (_DataRequest.getAutoIndex())
            {
                _InitialAutoIndex();
                if (_Mod[stMod::ERROR] == stMod::ERROR)
                    _Mod[stMod::AUTOINDEX] = stMod::EMPTY;
                else
                    _ErrorPage.SetAutoIndex(_DataRequest.getAutoIndex());
            }
            if (_Mod[stMod::ERROR] != stMod::ERROR)
                _ErrorPage.ResponseError(_Status, _DataRequest.getPhysicalPath());
            else
                _ErrorPage.ResponseError(_Status, "");
            _Mod[stMod::ERROR] = stMod::EMPTY;
        }
        else
            _ErrorPage.ResponseError(_Status, "");
        _ModTransferData = true;
        _BodySize = _ErrorPage.GetBodySize();
        _BodyPointer = &_ErrorPage.GetBody();
        _HeaderFeildPointer = &_ErrorPage.GetHeaderField();
        _FileFromDiskPointer = &_ErrorPage.GetFileFromDisk();
        _IsConnection = _ErrorPage.GetIsConnection();
    }
}

void clsResponse::_StatusLine()
{
    _HeaderFeild += "HTTP/1.1 ";
    HelperFunctions::NumToStr(_Status, _HeaderFeild);
    _HeaderFeild += " ";
    _HeaderFeild += HelperFunctions::GetStatusMessage(_Status);
    _HeaderFeild += "\r\n";
}

void clsResponse::_ContentLength()
{
    _HeaderFeild += "Content-Length: ";
    HelperFunctions::NumToStr(_BodySize, _HeaderFeild);
    _HeaderFeild += "\r\n";
}
void clsResponse::_ContentType()
{
    _HeaderFeild += "Content-Type: ";
    _HeaderFeild += _Type;
    _HeaderFeild += " ; charset=UTF-8\r\n";
}

void clsResponse::_Connection(bool Isclose)
{
    if (Isclose)
        _HeaderFeild += "Connection: keep-alive\r\n";
    else
        _HeaderFeild += "Connection: Close\r\n";
}
void clsResponse::_Transfer_Encoding()
{
    _HeaderFeild += "Transfer-Encoding: chunked\r\n";
}

void clsResponse::_Redirction()
{
    _HeaderFeild += "Location: ";
    _HeaderFeild += _DataRequest.getReturn().value.raw_path;
    _HeaderFeild += "\r\n";
}
void clsResponse::_Date()
{
    _HeaderFeild += "Date: ";
    _HeaderFeild += HelperFunctions::DateTime();
    _HeaderFeild += "\r\n";
}

void clsResponse::_Server()
{
    _HeaderFeild += "Server: HTTP/1.1\r\n";
}

void clsResponse::_StoredInFileOrStr()
{
    struct stat MetaData;
    if (_FileFromDisk.empty())
        return;
    if (stat(_FileFromDisk.c_str(), &MetaData) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        return;
    }
    _BodySize = MetaData.st_size;
    if (_BodySize > MAX_BODY)
    {
        _Mod[stMod::CHUNK] = stMod::CHUNK;
        return;
    }
    int FD = open(_FileFromDisk.c_str(), O_RDONLY | O_CLOEXEC, 0644);
    if (FD < 0)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        return;
    }
    if (read(FD, &_Body[0], MAX_BODY) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        close(FD);
        return;
    }
    _FileFromDisk.clear();
    close(FD);
}

void clsResponse::SetStatus(short Status)
{
    _Status = Status;
}

void clsResponse::SetMod(stMod::eMod Mod)
{
    _Mod[Mod] = Mod;
}

void clsResponse::Reset()
{

    _Status = 0;
    _BodySize = 0;
    _FileFromDisk = "";
    _HeaderFeild = "";
    _ModTransferData = false;
    _Erno = false;
    _SizeHeaders = 0;
    _Type = "";
    _IsConnection = true;
    _ErrorPage.Reset();
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, sizeof(_Mod));
}

bool clsResponse::GetIsConnection() const
{
    return _IsConnection;
}

const std::string &clsResponse::GetHeaderFeild() const
{
    return _HeaderFeild;
}
const std::string &clsResponse::GetBody() const
{
    return _Body;
}

const std::string &clsResponse::GetFileName() const
{
    return _FileFromDisk;
}

const std::string *clsResponse::GetBodyPointer()
{
    return _BodyPointer;
}
const std::string *clsResponse::GetHeaderFeildPointer()
{
    return _HeaderFeildPointer;
}
const std::string *clsResponse::GetFileFromDiskPointer() const
{
    return _FileFromDiskPointer;
}

void clsResponse::SetModTransferData(bool ModTransferData)
{
    _ModTransferData = ModTransferData;
}

void clsResponse::SetBodyPointer(const std::string *BodyPointer)
{
    _BodyPointer = BodyPointer;
}
void clsResponse::SetHeaderFeildPointer(const std::string *HeaderFeildPointer)
{
    _HeaderFeildPointer = HeaderFeildPointer;
}
void clsResponse::SetFileFromDiskPointer(const std::string *FileFromDiskPointer)
{
    _FileFromDiskPointer = FileFromDiskPointer;
}

void clsResponse::SetInternalRedirectSrc(const std::string &InternalRedirectSrc)
{
    _InternalRedirectSrc = InternalRedirectSrc;
}
std::string &clsResponse::GetInternalRedirectSrc()
{
    return _InternalRedirectSrc;
}
bool clsResponse::GetModTransferData() const
{
    return _ModTransferData;
}
void clsResponse::SetSizeBody(int size)
{
    _BodySize = size;
}

void clsResponse::SetIsConnection(bool IsConnection)
{
    _IsConnection = IsConnection;
}
short clsResponse::GetStatus()
{
    return _Status;
}
size_t clsResponse::GetSizeBody() const
{
    return (_BodySize);
}
bool clsResponse::GetErnoVar()
{
    return _Erno;
}
bool clsResponse::IsAutoIndex()
{
    return (_Mod[stMod::AUTOINDEX] == stMod::AUTOINDEX);
}
bool clsResponse::fetchAutoIndex(char *Buffer, short &Ofset, short LimitSize)
{
    flowAutoIndex FlagAutoIndex = AutoIndex.insertAutoDirective(Buffer, Ofset, LimitSize);
    if (FlagAutoIndex == ERROR_AUTO_INDEX)
    {
        _IsConnection = false;
        return true;
    }
    else if (FlagAutoIndex == DONE_AUTO_INDEX)
        return true;
    return false;
}
bool clsResponse::IsError()
{
    return (_Mod[stMod::ERROR] == stMod::ERROR);
}
clsResponse::~clsResponse() {}