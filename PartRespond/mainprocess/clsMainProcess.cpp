/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yadib <yadib@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:43:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/28 12:59:49 by yadib            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"

// yadib modifier this part of achraf
clsMainProcess::clsMainProcess(RequestHandler &RequestLinker) : _Response(RequestLinker), _CGI(RequestLinker) ,_DataRequest(RequestLinker) 
{
    _RunCGI = false;
}
clsMainProcess::~clsMainProcess() {} // free right way

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
    {
        parseCgi.SetProcessIsFinish(true);
    }

    if (Length > 0 || _eventProcess == stEventProcess::THE_END)
        parseCgi.ReceivingData(Buffer, Length);
    if (parseCgi.GetMod()[stMod::ERROR] == stMod::ERROR || _eventProcess == stEventProcess::THE_END)
    {
        if(parseCgi.GetMod()[stMod::ERROR] == stMod::ERROR)
        {
            _eventProcess = stEventProcess::END_WITH_PARSE;
            _Response.SetBodyPointer(parseCgi.GetBodyPointer());
            _Response.SetHeaderFeildPointer(parseCgi.GetHeaderFeildPointer());
            _Response.SetFileFromDiskPointer(parseCgi.GetFileFromDiskPointer());
            _Response.SetSizeBody(parseCgi.GetSizeBody());
            _Response.SetStatus(parseCgi.GetStatus());
            _Response.SetIsConnection(parseCgi.GetIsConnection());
            _Response.SetModTransferData(true);
        }
        else
        {
            _Response.SetBodyPointer(&parseCgi.GetBody());
            _Response.SetHeaderFeildPointer(&parseCgi.GetHeadersFieldFinal());
            _Response.SetFileFromDiskPointer(&parseCgi.GetFileNameBody());
            _Response.SetInternalRedirectSrc(parseCgi.GetInternalRedirectSrc());
            _Response.SetSizeBody(parseCgi.GetSizeBody());
            _Response.SetModTransferData(true);
        }
    }
    else if (_eventProcess == stEventProcess::END_WITH_TIMOUT || _eventProcess == stEventProcess::END_UNKNOW)
    {
        _ErrorPage.ResponseError(_eventProcess, "");
        _Response.SetBodyPointer(&_ErrorPage.GetBody());
        _Response.SetHeaderFeildPointer(&_ErrorPage.GetHeaderField());
        _Response.SetFileFromDiskPointer(&_ErrorPage.GetFileFromDisk());
        _Response.SetSizeBody(_ErrorPage.GetBodySize());
        _Response.SetStatus(_eventProcess);
        _Response.SetIsConnection(_ErrorPage.GetIsConnection());
        _Response.SetModTransferData(true);
    }
}

void clsMainProcess::_InitializeCGI()
{
    if (!_RunCGI)
    {
        _CGI.RunCGI();
        if (!_CGI.GetErno())
        {
            std::cout << _RunCGI<<" initialize cgi\n" << std::endl;
            _RunCGI = _CGI.GetIsRunCGI();
            _eventProcess = stEventProcess::RUNINNG;
        }
    }
   if (_CGI.GetErno())
   {
        _eventProcess = stEventProcess::END_UNKNOW;
        _ErrorPage.ResponseError(500, "");
        _Response.SetBodyPointer(&_ErrorPage.GetBody());
        _Response.SetHeaderFeildPointer(&_ErrorPage.GetHeaderField());
        _Response.SetFileFromDiskPointer(&_ErrorPage.GetFileFromDisk());
        _Response.SetModTransferData(true);
   }
   
}

void clsMainProcess::_PartDeleteMethod()
{
    _Response.SetMod(stMod::DELETE);
    _Response.SetStatus(200);
    _Response.MakeResponse();
}

void clsMainProcess::_PartPOSMethod()
{
    _Response.SetStatus(200);
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
    _Response.SetStatus(_DataRequest.getStatusError());
    _Response.MakeResponse();
}

void clsMainProcess::MainProcess()
{
    _RunCGI = false;
    if(_DataRequest.getStatusError())
        _PartErrorRequest();
    else if (_DataRequest.getPathCgi())
        _InitializeCGI();
    else if (_DataRequest.getReturn().value.compare("") != 0)
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
    _CGI.Reset();
    _CGI.GetclsParseOutCGI().Reset();
    _Response.Reset(); // reset error page also
    _eventProcess = stEventProcess::RUNINNG; // add by adib
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