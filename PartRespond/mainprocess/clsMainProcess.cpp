/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:43:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/15 19:02:14 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"

// yadib modifier this part of achraf
clsMainProcess::clsMainProcess(RequestHandler &RequestLinker) : _DataRequest(RequestLinker), _Response(RequestLinker) {}
clsMainProcess::~clsMainProcess() {}

void clsMainProcess::_PartRedirection()
{
    std::cout << _DataRequest.getReturn().code << std::endl;
    _Response.SetStatus(_DataRequest.getReturn().code);
    _Response.SetMod(stMod::REDIRECTION);
    // modfier by yadib
    // _Response.SetRequestHandler(_DataRequest);
    _Response.MakeResponse();
}

void clsMainProcess::_PartCGI()
{
    _Response.SetStatus(200);
    // Cgi hear
}

void clsMainProcess::_PartDeleteMethod()
{
    _Response.SetMod(stMod::DELETE);
    _Response.SetStatus(200);
    if (access(_DataRequest.getPhysicalPath(), R_OK))
    {
        _Response.SetMod(stMod::ERROR);
        _Response.SetStatus(404);
        _Response.MakeResponse();
    }
    // // delete file or folder any things
    // modfier by yadib
    // _Response.SetRequestHandler(_DataRequest);
    _Response.MakeResponse();
}

void clsMainProcess::_PartPOSMethod()
{
    // remove set request handler function by adib
    _Response.SetStatus(200);
    _Response.SetMod(stMod::UPLOAD);
    _Response.MakeResponse();
}

void clsMainProcess::_PartGETMethod()
{
    // remove set request handler function by adib

    _Response.SetMod(stMod::GET);
    _Response.SetStatus(200);
    _Response.MakeResponse();
}

void clsMainProcess::_PartErrorRequest()
{
    // remove set request handler function by adib

    _Response.SetMod(stMod::ERROR);
    _Response.SetStatus(_DataRequest.getError().getCodeStatus());
    _Response.MakeResponse();
}

void clsMainProcess::MainProcess()
{
    // bool check = false;
    // _DataRequest = DataRequest;
    if (_DataRequest.getError().isError())
        _PartErrorRequest();
    else if (_DataRequest.getReturn().value.compare("") != 0)
        _PartRedirection();
    else if ((_DataRequest.getMethod() == HttpTables::M_GET))
        _PartGETMethod();
    // else if (_DataRequest.getPathCgi() != NULL && *(_DataRequest.getPathCgi()) != "") // PROBLEM HERE
    //     _PartCGI();
    else if ((_DataRequest.getMethod() == HttpTables::M_DELETE))
        _PartDeleteMethod();
    else if ((_DataRequest.getMethod() == HttpTables::M_POST))
        _PartPOSMethod();
}

clsResponse &clsMainProcess::GetclsResponse()
{
    return _Response;
}