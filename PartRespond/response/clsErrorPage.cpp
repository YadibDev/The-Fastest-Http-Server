/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsErrorPage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:48:27 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/12 16:33:19 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsErrorPage.hpp"

 clsErrorPage::clsErrorPage()
{
    _Type = "";
    _Status = 0;
    _BodySize = 0;
    _HeaderFeild.resize(4000);
}

void clsErrorPage::SetType(std::string Type) 
{
    _Type = Type;
}

std::string &clsErrorPage::HeadersErrorResponse()
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
    return (_HeaderFeild);
}
 
std::string &clsErrorPage::ResponseError(int Status)
{
   if (Status >= 0)
        _Status = Status;
//    if (!_BodySize)
//         _BodySize = HelperFunctions::GetBody(Status).size();
   return (HeadersErrorResponse());
}
 
void clsErrorPage::ConnectionClose()
{
    _HeaderFeild += "Connection: Close\r\n";
}

void clsErrorPage::Status()
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
void clsErrorPage::ContentType()
{
    _HeaderFeild += "Content-Type: ";
    _HeaderFeild += _Type;
    _HeaderFeild +="\r\n";
}

void clsErrorPage::Date()
{
    _HeaderFeild += "Date: ";
    _HeaderFeild += HelperFunctions::DateTime();
    _HeaderFeild += "\r\n";     
}

void clsErrorPage::Server()
{
    _HeaderFeild += "Server: Name Server\r\n";
}
void clsErrorPage::RetryAfter()
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

void clsErrorPage::Transfer_Encoding()
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