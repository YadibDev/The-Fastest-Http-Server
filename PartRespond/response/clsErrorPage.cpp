/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsErrorPage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:48:27 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/16 15:56:38 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsErrorPage.hpp"

 clsErrorPage::clsErrorPage()
{
    _Type = "";
    _Status = 0;
    _BodySize = 0;
    _Type.resize(500);
    _FileFromDisk.resize(1000);
    _HeaderFeild.resize(4000);
    _Body.resize(40000);
    if (_Type.empty() || _HeaderFeild.empty() || _FileFromDisk.empty() || _Body.empty())
    {
        _Mod[stMod::ERROR] == stMod::ERROR;
        _Status = 500;
        return ;
    }
    _FileFromDisk.clear();
    _Type.clear();
    _HeaderFeild.clear();
    _Erno = false;
}

void clsErrorPage::SetType(std::string Type) 
{
    _Type = Type;
}

void clsErrorPage::_HeadersErrorResponse()
{
    Status();
    ContentType();
    if (_Mod[stMod::CHUNK] != stMod::CHUNK)
        ContentLength();
    Server();
    Date();
    if (_Status == 405)
       Allow();
    if (_Mod[stMod::CHUNK] == stMod::CHUNK)
        Transfer_Encoding();
    if (_Status == 429 || _Status == 503)
        RetryAfter();
    ConnectionClose();
    _HeaderFeild += "\r\n";
}
 
void clsErrorPage::_StoredInFileOrStr()
{
    struct stat MetaData;
    _Body.clear();
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
    if (read(FD,&_Body[0],40000) == -1)
    {
        _Mod[stMod::ERROR] = stMod::ERROR;
        _Status = 500;
        _Erno = true;
        close(FD);
        return ;
    }
    _FileFromDisk = "";
    close(FD);
}
 
void clsErrorPage::ResponseError(int Status, const std::string &FilePageError)
{
    _Status = Status;
    short PrevStatus = Status;
    if (!FilePageError.empty())
    {
        _FileFromDisk = FilePageError;
        _StoredInFileOrStr();
        if (_Erno)
        {
            Type = ".html";
            _BodySize = HelperFunctions::ft_strlen( HelperFunctions::GetBody(PrevStatus));
            _Body = HelperFunctions::GetBody(PrevStatus);
            _HeadersErrorResponse();
            return ;
        }
        else
        {
           _Type = HelperFunctions::GetType(HelperFunctions::GetTypeDataFile(_FileFromDisk)); 
            _HeadersErrorResponse();
            return ;
        }
    }
    Type = ".html";
    _BodySize = HelperFunctions::ft_strlen( HelperFunctions::GetBody(Status));
    _Body = HelperFunctions::GetBody(Status);
    _HeadersErrorResponse();
}
 
void clsErrorPage::_ConnectionClose()
{
    _HeaderFeild += "Connection: Close\r\n";
}

void clsErrorPage::_Status()
{
    char *Number = HelperFunctions::ft_itoa(_Status);
    _HeaderFeild += "HTTP/1.1 ";
    _HeaderFeild += Number ;
    _HeaderFeild +=  " ";
    _HeaderFeild += HelperFunctions::GetStatusMessage(_Status) ;
    _HeaderFeild += "\r\n";
    delete[] Number;
}

void clsErrorPage::ContentLength()
{
     char *Number = HelperFunctions::ft_itoa(_BodySize);
    _HeaderFeild += "Content-Length: ";
    _HeaderFeild += Number ;
    _HeaderFeild +="\r\n";
    delete[] Number;
}
void clsErrorPage::_ContentType()
{
    _HeaderFeild += "Content-Type: ";
    _HeaderFeild += _Type;
    _HeaderFeild +="\r\n";
}

void clsErrorPage::_Date()
{
    _HeaderFeild += "Date: ";
    _HeaderFeild += HelperFunctions::DateTime();
    _HeaderFeild += "\r\n";     
}

void clsErrorPage::Server()
{
    _HeaderFeild += "Server: Name Server\r\n";
}
void clsErrorPage::_RetryAfter()
{
    char *Number = HelperFunctions::ft_itoa(120);
    _HeaderFeild += "Retey-After: ";
    _HeaderFeild += Number;
    _HeaderFeild += "\r\n";
    delete[] Number;   
}
void clsErrorPage::Allow()
{
    _HeaderFeild += "Allow: GET, POST, DELETE\r\n";
}

void clsErrorPage::_Transfer_Encoding()
{
   _HeaderFeild += "Transfer-Encoding: chunked\r\n";
}

void clsErrorPage::SetMod(const stMod::eMod *Mod)
{
    for (int i = 0; i < 10; i++)
        _Mod[i] = Mod[i];
}

void clsErrorPage::SetBodySize(int BodySize)
{
    _BodySize = BodySize;
}

clsErrorPage::~clsErrorPage()
{
    _Type = "";
    _HeaderFeild = "";
    _BodySize = 0;
    _Status = 0;
}