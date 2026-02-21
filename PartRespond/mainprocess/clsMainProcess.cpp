/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:43:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/20 21:41:41 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"

clsMainProcess::clsMainProcess(){}
clsMainProcess::~clsMainProcess(){}

void clsMainProcess::_PartRedirection()
{
    _Response.SetStatus(_DataRequest.return_status);
    _Response.SetMod(REDIRECTION);
    _Response.MakeResponse();
}

void clsMainProcess::_PartPermission()
{
    _Response.SetStatus(403);
    _Response.SetMod(ERROR);
    _Response.MakeResponse();
}

void clsMainProcess::_PartCGI()
{
    _Response.SetStatus(200);
    
}

void clsMainProcess::_PartDeleteMethod()
{
    _Response.SetMod(DELETEM);
    _Response.SetStatus(200);
    if (access(_DataRequest.getPhysicalPath().c_str(), F_OK))
    {
        _Response.SetMod(ERROR);
        _Response.SetStatus(404);
        _Response.MakeResponse();
    }
    // // delete file or folder any things
    _Response.MakeResponse();
} 

void clsMainProcess::_PartPOSMethod()
{
    _Response.SetStatus(200);
    _Response.SetMod(UPLOAD);
    _Response.MakeResponse();
}

void clsMainProcess::_PartGETMethod()
{
    if (access(_DataRequest.getPhysicalPath().c_str(), F_OK))
    {
        _Response.SetMod(ERROR);
        _Response.SetStatus(404);
        _Response.SetRequestHandler(_DataRequest);
        _Response.MakeResponse();
    }
    else
    {
        _Response.SetMod(GETM);
        _Response.SetStatus(200);
        _Response.SetRequestHandler(_DataRequest);
        _Response.MakeResponse();
    }
}

void clsMainProcess::MainProcess(const RequestHandler &DataRequest)
{
    bool check = false;
    _DataRequest = DataRequest;
    if (_DataRequest.return_url != "")
        _PartRedirection();
    else if (check != 0)
        _PartPermission();
    else if ((_DataRequest.getMethod() ==GET))
        _PartGETMethod();
    else if (check != 0)
        _PartCGI();
    else if ((_DataRequest.getMethod() == DELETE))
        _PartDeleteMethod();
    else if ((_DataRequest.getMethod() == POST))
        _PartPOSMethod();
        
}

clsResponse clsMainProcess::GetclsResponse()
{
    return _Response;
}