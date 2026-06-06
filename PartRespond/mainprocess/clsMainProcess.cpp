#include "../mainprocess/Webserv.hpp"

clsMainProcess::clsMainProcess(RequestHandler &RequestLinker)
    : _Response(RequestLinker, _Body, _HeaderFeild, _FileFromDisk, _Type),
      _CGI(RequestLinker, _Body, _HeaderFeild, _FileFromDisk, _InternalRedirectSrc), _DataRequest(RequestLinker)
{
    _Body.resize(MAX_BODY);
    _InternalRedirectSrc.resize(MAX_HEADERS);
    _HeaderFeild.resize(MAX_HEADERS);
    _FileFromDisk.resize(PATH_MAX);
    _Type.resize(100);
    if (_Type.empty() || _FileFromDisk.empty() || _HeaderFeild.empty() || _InternalRedirectSrc.empty() || _Body.empty())
    {
        _Response.SetMod(stMod::MEMORY_FAILD);
        _Response.SetIsConnection(false);
        return;
    }
    _InternalRedirectSrc = "";
    _HeaderFeild = "";
    _FileFromDisk = "";
    _Type = "";
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
    if (parseCgi.GetMod()[stMod::MEMORY_FAILD] == stMod::MEMORY_FAILD)
    {
        _eventProcess = stEventProcess::END_WITH_PARSE;
        _Response.SetIsConnection(false);
        return;
    }
    if (_eventProcess == stEventProcess::THE_END)
        parseCgi.SetProcessIsFinish(true);
    if (Length > 0 || _eventProcess == stEventProcess::THE_END)
        parseCgi.ReceivingData(Buffer, Length);
    if (parseCgi.GetMod()[stMod::ERROR] == stMod::ERROR || _eventProcess == stEventProcess::THE_END)
    {
        if (parseCgi.GetMod()[stMod::ERROR] == stMod::ERROR)
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
            _Response.SetIsConnection(parseCgi.GetIsConnection());
        }
    }
    else if (_eventProcess == stEventProcess::END_WITH_TIMOUT || _eventProcess == stEventProcess::END_UNKNOW)
    {
        _Response.SetMod(stMod::ERROR);
        _Response.SetStatus(_eventProcess);
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
    _Response.SetMod(stMod::UPLOAD);
    _Response.SetStatus(201);
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
    if (!_Response.GetIsConnection())
        return;
    if (_DataRequest.getPathCgi())
        _InitializeCGI();
    else if (_DataRequest.getStatusError())
        _PartErrorRequest();
    else if (_DataRequest.getReturn().code != -1)
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
    _InternalRedirectSrc = "";
    _HeaderFeild = "";
    _FileFromDisk = "";
    _Type = "";
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