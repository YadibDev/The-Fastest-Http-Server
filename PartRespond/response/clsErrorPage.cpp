/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsErrorPage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:48:27 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/13 11:41:57 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsErrorPage.hpp"

clsErrorPage::clsErrorPage(std::string &Body , std::string &HeaderFeild, std::string &FileFromDisk, std::string &Type)
    :_Body(Body), _HeaderFeild(HeaderFeild), _FileFromDisk(FileFromDisk), _Type(Type)
{
    _Status = 0;
    _IsConnection = false;
    _BodySize = 0;
    _IsAutoIndex = false;
    _Erno = false;
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, sizeof(_Mod));
}

void clsErrorPage::_HeadersErrorResponse()
{
    _StatusLine();
    _ContentType();
    if (_Mod[stMod::CHUNK] != stMod::CHUNK)
        _ContentLength();
    if (_Status == 405)
        _Allow();
    if (_Mod[stMod::CHUNK] == stMod::CHUNK)
        _Transfer_Encoding();
    if (_Status == 429 || _Status == 503)
        _RetryAfter();
    _CheckConnection();
    _Connection();
    _Server();
    _Date();
    _HeaderFeild += "\r\n";
}

void clsErrorPage::_StoredInFileOrStr(size_t sizeBody)
{
    if (_FileFromDisk.empty())
        return;
    _BodySize = sizeBody;
    if (_BodySize > MAX_BODY)
    {
        _Mod[stMod::CHUNK] = stMod::CHUNK;
        return;
    }
    int FD = open(_FileFromDisk.c_str(), O_RDONLY | O_CLOEXEC);
    if (FD < 0)
    {

        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        return;
    }
    if (read(FD, &_Body[0], MAX_BODY) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        close(FD);
        return;
    }
    _FileFromDisk = "";
    close(FD);
}

void clsErrorPage::ResponseError(int Status, const std::string &FilePageError, size_t sizeBody)
{
    _Status = Status;
    short i = 0 ;
    if (_IsAutoIndex)
    {
        _FileFromDisk = "";
        _Type = HelperFunctions::GetType(".html");
        _Mod[stMod::CHUNK] = stMod::CHUNK;
        _HeadersErrorResponse();
        return ;
    }
    if (!FilePageError.empty())
    {
        _FileFromDisk = FilePageError;

        _StoredInFileOrStr(sizeBody);
        if (_Erno)
        {
            _Type = HelperFunctions::GetType(".html");
            _FileFromDisk = "";
            _BodySize = HelperFunctions::ft_strlen(HelperFunctions::GetBody(_Status));
            HelperFunctions::ft_str_copy(&_Body[0], HelperFunctions::GetBody(_Status), MAX_BODY,i, _BodySize, 0);
            _Body = HelperFunctions::GetBody(_Status);
            _HeadersErrorResponse();
            return;
        }
        else
        {
            _Type = HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(FilePageError));
            _HeadersErrorResponse();
            return;
        }
    }
    _Type = HelperFunctions::GetType(".html");
    _FileFromDisk = "";
    _BodySize = HelperFunctions::ft_strlen(HelperFunctions::GetBody(Status));
    HelperFunctions::ft_str_copy(&_Body[0], HelperFunctions::GetBody(_Status), MAX_BODY,i, _BodySize, 0);
    _HeadersErrorResponse();
}

void clsErrorPage::_Connection()
{
    if (_IsConnection)
        _HeaderFeild += "Connection: keep-alive\r\n";
    else
        _HeaderFeild += "Connection: Close\r\n";
}

void clsErrorPage::_CheckConnection()
{
    if (_Status == 404)
        _IsConnection = true;
}

void clsErrorPage::_StatusLine()
{
    _HeaderFeild += "HTTP/1.1 ";
    HelperFunctions::NumToStr(_Status, _HeaderFeild);
    _HeaderFeild += " ";
    _HeaderFeild += HelperFunctions::GetStatusMessage(_Status);
    _HeaderFeild += "\r\n";
}

void clsErrorPage::_ContentLength()
{
    _HeaderFeild += "Content-Length: ";
    HelperFunctions::NumToStr(_BodySize, _HeaderFeild);
    _HeaderFeild += "\r\n";
}
void clsErrorPage::_ContentType()
{
    _HeaderFeild += "Content-Type: ";
    _HeaderFeild += _Type;
    _HeaderFeild += "\r\n";
}

void clsErrorPage::_Date()
{
    _HeaderFeild += "Date: ";
    HelperFunctions::DateTime(_HeaderFeild);
    _HeaderFeild += "\r\n";
}

void clsErrorPage::_Server()
{
    _HeaderFeild += "Server: the-fastest-server\r\n";
}
void clsErrorPage::_RetryAfter()
{
    _HeaderFeild += "Retey-After: ";
    HelperFunctions::NumToStr(120, _HeaderFeild);
    _HeaderFeild += "\r\n";
}
void clsErrorPage::_Allow()
{
    _HeaderFeild += "Allow: GET, POST, DELETE\r\n";
}

void clsErrorPage::_Transfer_Encoding()
{
    _HeaderFeild += "Transfer-Encoding: chunked\r\n";
}

void clsErrorPage::SetBodySize(int BodySize)
{
    _BodySize = BodySize;
}

const std::string &clsErrorPage::GetHeaderField()
{
    return (_HeaderFeild);
}

const std::string &clsErrorPage::GetBody()
{
    return (_Body);
}
const std::string &clsErrorPage::GetFileFromDisk()
{
    return (_FileFromDisk);
}

clsErrorPage::~clsErrorPage()
{
    _Type = "";
    _HeaderFeild = "";
    _BodySize = 0;
    _Status = 0;
    _Erno = false;
    _FileFromDisk = "";
}

void clsErrorPage::Reset()
{
    _Type = "";
    _HeaderFeild = "";
    _BodySize = 0;
    _Status = 0;
    _Erno = false;
    _FileFromDisk = "";
    _Body = "";
    _IsAutoIndex = false;
    HelperFunctions::ft_memset(&_Mod, stMod::EMPTY, sizeof(_Mod));
}

bool clsErrorPage::GetIsConnection()
{
    return _IsConnection;
}
 void clsErrorPage::SetAutoIndex(bool IsAutoIndex)
 {
    _IsAutoIndex = IsAutoIndex;
 }
 size_t clsErrorPage::GetBodySize() const
{
    return this->_BodySize;
}