/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:43:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/12 21:45:27 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"

// yadib modifier this part of achraf
clsMainProcess::clsMainProcess(RequestHandler &RequestLinker) 
    : _Response(RequestLinker, _Body, _HeaderFeild, _FileFromDisk, _Type), _CGI(RequestLinker, _HeaderFeild, _FileFromDisk, _InternalRedirectSrc) ,_DataRequest(RequestLinker) 
{
    _Body.resize(MAX_BODY);
	_InternalRedirectSrc.resize(1000);
    _HeaderFeild.resize(MAX_HEADERS);
	_FileFromDisk.resize(1000);
    _Type.resize(500);
    _RunCGI = false;
}
clsMainProcess::~clsMainProcess() {}

void clsMainProcess::_PartRedirection()
{
    _Response.SetStatus(_DataRequest.getReturn().code);
    _Response.SetMod(stMod::REDIRECTION);
    _Response.MakeResponse();
}

stEventProcess::eEventProcess &clsMainProcess::getEventProcess()
{
    return this->_eventProcess;
}
void clsMainProcess::setEventProcess(stEventProcess::eEventProcess ev)
{
    this->_eventProcess = ev;
}

void clsMainProcess::ParseCGI(const char *Buffer, short Length)
{
    clsParseOutCGI &parseCgi = _CGI.GetclsParseOutCGI();
    if (_eventProcess == stEventProcess::THE_END)
        parseCgi.SetProcessIsFinish(true);
    if (Length > 0 || _eventProcess == stEventProcess::THE_END)
        parseCgi.ReceivingData(Buffer, Length);
    if (parseCgi.GetMod()[stMod::ERROR] == stMod::ERROR || _eventProcess == stEventProcess::THE_END)
    {
        if(parseCgi.GetMod()[stMod::ERROR] == stMod::ERROR)
        {
            _eventProcess = stEventProcess::END_WITH_PARSE;
            _Response.SetMod(stMod::ERROR);
            _Response.SetStatus(parseCgi.GetStatus());
            _Response.MakeResponse();
        }
        else
        {
            _Response.SetBodyPointer(&parseCgi.GetBody());
            _Response.SetHeaderFeildPointer(&parseCgi.GetHeadersFieldFinal());
            _Response.SetFileFromDiskPointer(&parseCgi.GetFileNameBody());
            _Response.SetInternalRedirectSrc(&parseCgi.GetInternalRedirectSrc());
            _Response.SetSizeBody(parseCgi.GetSizeBody());
            _Response.SetModTransferData(true);
        }
    }
    else if (_eventProcess == stEventProcess::END_WITH_TIMOUT || _eventProcess == stEventProcess::END_UNKNOW)
    {
        _Response.SetStatus(_eventProcess);
        _Response.SetMod(stMod::ERROR);
        _Response.MakeResponse();
    }
}

void clsMainProcess::_InitializeCGI()
{
    if (!_RunCGI)
    {
        _CGI.RunCGI();
        if (!_CGI.GetErno())
        {
            _RunCGI = _CGI.GetIsRunCGI();
            _eventProcess = stEventProcess::RUNINNG;
        }
    }
   if (_CGI.GetErno())
   {
        _eventProcess = stEventProcess::END_UNKNOW;
        _Response.SetStatus(500);
        _Response.SetMod(stMod::ERROR);
        _Response.MakeResponse();
   }
   
}

void clsMainProcess::_PartDeleteMethod()
{
    _Response.SetMod(stMod::DELETE);
    _Response.SetStatus(204);
    _Response.MakeResponse();
}

void clsMainProcess::_PartPOSMethod()
{
    _Response.SetStatus(201);
    _Response.SetMod(stMod::UPLOAD);
    _Response.MakeResponse();
}

void clsMainProcess::_PartGETMethod()
{
    _Response.SetMod(stMod::GET);
    _Response.SetStatus(200);
    _Response.MakeResponse();
}

void clsMainProcess::_PartErrorRequest()
{
    _Response.SetMod(stMod::ERROR);
    _Response.SetMod(stMod::INTERNALRE);
    _Response.SetStatus(_DataRequest.getStatusError());
    _Response.MakeResponse();
}

void clsMainProcess::MainProcess()
{
    _RunCGI = false;
    if(_DataRequest.getStatusError() && _DataRequest.getPathCgi())
        _InitializeCGI();
    else if(_DataRequest.getStatusError())
        _PartErrorRequest();
    else if (_DataRequest.getPathCgi())
        _InitializeCGI();
    else if (_DataRequest.getReturn().value.raw_path.compare("") != 0)
        _PartRedirection();
    else if ((_DataRequest.getMethod() == HttpTables::M_GET))
        _PartGETMethod();
    else if ((_DataRequest.getMethod() == HttpTables::M_DELETE))
        _PartDeleteMethod();
    else if ((_DataRequest.getMethod() == HttpTables::M_POST))
        _PartPOSMethod();
}

void clsMainProcess::Reset()
{
    this->_RunCGI = false;
    _CGI.Reset();
    _CGI.GetclsParseOutCGI().Reset();
    _Response.Reset();
    _eventProcess = stEventProcess::RUNINNG; 
}

clsCGI &clsMainProcess::GetclsCGI()
{
    return _CGI;
}

bool clsMainProcess::GetIsRunCGI()
{
    return _RunCGI;
}

clsResponse &clsMainProcess::GetclsResponse()
{
    return _Response;
}

bool clsMainProcess::isRunCgi()
{
    return _RunCGI;
}