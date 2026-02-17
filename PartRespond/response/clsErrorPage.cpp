/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsErrorPage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:48:27 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/17 14:52:52 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsErrorPage.hpp"

 clsErrorPage::clsErrorPage()
{
    _Type = "";
    _Status = 0;
    StoredBodys();
    StoredMessage();
}

void clsErrorPage::StoredBodys()
 {
     _Body[200] = "Body200";
     _Body[201] = "Body201";
     _Body[204] = "Body204";
     _Body[301] = "Body301";
     _Body[302] = "Body302";
     _Body[400] = "Body400";
     _Body[403] = "Body403";
     _Body[404] = "Body404";
     _Body[500] = "Body500";
     _Body[501] = "Body501";
 }
 
 void clsErrorPage::StoredMessage()
 {
     _Message[200] = "OK";
     _Message[201] = "Created";
     _Message[204] = "No Content";
     _Message[301] = "Moved Permanently";
     _Message[302] = "found";
     _Message[400] = "Bad Request";
     _Message[403] = "Forbidden";
     _Message[404] = "Not Found";
     _Message[500] = "Internal Server Error";
     _Message[501] = "Not Implemented";
 }

std::string clsErrorPage::GetStatusMessage(int Status) 
{
    if (_Message.count(Status))
        return  _Message[Status];
    return ("Unknown Status");
}
void clsErrorPage::SetType(std::string Type) 
{
    _Type = Type;
}
 std::string clsErrorPage::GetBody(int Status)
 {
     if (_Body.count(Status))
         return  _Body[Status];
     return ("Unknown Status");
 }
 std::string clsErrorPage::HeadersErrorResponse()
 {
    Status();
    ContentType();
    ContentLength();
    Server();
    Date();
    if (_Status == 405)
       Allow();
    if (_Status == 429 || _Status == 503)
        RetryAfter();
    ConnectionClose();
    _HeaderFeild += "\r\n";
    return (_HeaderFeild);
 }
 
 std::string clsErrorPage::ResponseError(int Status)
 {
    if (Status >= 0)
        _Status = Status;
    _BodySize = GetBody(Status).size();
    return (HeadersErrorResponse());
 }
 
void clsErrorPage::ConnectionClose()
{
    _HeaderFeild += "Connection: Close\r\n";
}

void clsErrorPage::Status()
{
    std::stringstream Headers;
    Headers << "HTTP/1.1 "<< _Status << " " <<  GetStatusMessage(_Status) <<"\r\n";
    _HeaderFeild += Headers.str();
}

void clsErrorPage::ContentLength()
{
    std::stringstream Headers;
    Headers << "Content-Length: "<< _BodySize<<"\r\n";
    _HeaderFeild += Headers.str();
}
void clsErrorPage::ContentType()
{
    std::stringstream Headers;
    _HeaderFeild += "Content-Type: ";
    _HeaderFeild += _Type;
    _HeaderFeild +="\r\n";
}

void clsErrorPage::Date()
{
    _HeaderFeild += "Date: ";
    _HeaderFeild += DateTime();
    _HeaderFeild += "\r\n";     
}

void clsErrorPage::Server()
{
    _HeaderFeild += "Server: Name Server\r\n";
}
void clsErrorPage::RetryAfter()
{
    std::stringstream Headers;
    Headers << "Retey-After: "<< 120 << "\r\n";
    _HeaderFeild += Headers.str();
}
void clsErrorPage::Allow()
{
    _HeaderFeild += "Allow: GET, POST, DELETE\r\n";
}
clsErrorPage::~clsErrorPage()
{
    _Type = "";
    _HeaderFeild = "";
    _Status = 0;
}