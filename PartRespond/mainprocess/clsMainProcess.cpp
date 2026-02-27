/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yadib <yadib@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:43:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/27 01:52:26 by yadib            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"

clsMainProcess::clsMainProcess(){}
clsMainProcess::~clsMainProcess(){}

void clsMainProcess::_PartRedirection()
{
    _Response.SetStatus(_DataRequest.getReturn().code);
    _Response.SetMod(REDIRECTION);
    _Response.SetRequestHandler(_DataRequest);
    _Response.MakeResponse();
}

void clsMainProcess::_PartPermission()
{
    _Response.SetStatus(403);
    _Response.SetMod(ERROR);
    _Response.SetRequestHandler(_DataRequest);
    _Response.MakeResponse();
}

void clsMainProcess::_PartCGI()
{
    _Response.SetStatus(200);
    //Cgi hear
}

void clsMainProcess::_PartDeleteMethod()
{
    _Response.SetMod(DELETE);
    _Response.SetStatus(200);
    if (access(_DataRequest.getPhysicalPath().c_str(), R_OK))
    {
        _Response.SetMod(ERROR);
        _Response.SetStatus(404);
        _Response.MakeResponse();
    }
    // // delete file or folder any things
    _Response.SetRequestHandler(_DataRequest);
    _Response.MakeResponse();
} 

void clsMainProcess::_PartPOSMethod()
{
    _Response.SetStatus(200);
    _Response.SetMod(UPLOAD);
    _Response.SetRequestHandler(_DataRequest);
    _Response.MakeResponse();
}

void clsMainProcess::_PartGETMethod()
{
    _Response.SetMod(GET);
    _Response.SetStatus(200);
    _Response.SetRequestHandler(_DataRequest);
    _Response.MakeResponse();
}

void clsMainProcess::_PartErrorRequest()
{
    _Response.SetMod(ERROR);
    _Response.SetStatus(_DataRequest.getError().getCodeStatus());
    _Response.SetRequestHandler(_DataRequest);
    _Response.MakeResponse();
}

void clsMainProcess::MainProcess(const RequestHandler &DataRequest)
{
    _DataRequest = DataRequest;
    if (_DataRequest.getError().isError())
        _PartErrorRequest();
    else if (_DataRequest.getReturn().value != "")
        _PartRedirection();
    else if (!_DataRequest.MethodAllowed())
        _PartPermission();
    else if ((_DataRequest.getMethod() == "GET"))
        _PartGETMethod();
    else if (_DataRequest.getPathCgi() != "")
        _PartCGI();
    else if ((_DataRequest.getMethod() == "DELETE"))
        _PartDeleteMethod();
    else if ((_DataRequest.getMethod() == "POST"))
        _PartPOSMethod();
        
}

clsResponse &clsMainProcess::GetclsResponse()
{
    return _Response;
}