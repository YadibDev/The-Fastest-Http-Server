/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:40:02 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/11 15:15:00 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsCGI.hpp"
short clsCGI::_LimitProcess;
clsCGI::clsCGI(RequestHandler &DataRequest) : _DataRequest(DataRequest), _ParseOutCGI(DataRequest)
{
    _IsRunCGI = false;
    _Counter = 0;
    _Offset = 0;
    _Erno = false;
    _pip[0] = -1;
    _pip[1] = -1;
}

bool clsCGI::_MakeEnv()
{
    if (!_SERVER_SOFTWARE())
        return false;
    if (!_SERVER_NAME())
        return false;
    if (!_SERVER_PROTOCOL())
        return false;
    if (!_GATEWAY_INTERFACE())
        return false;
    if (!_REMOTE_IDENT())
        return false;
    if (!_REMOTE_HOST())
        return false;
    if (!_REMOTE_ADDR())
        return false;
    if (!_AUTH_TYPE())
        return false;
    if (!_REMOTE_USER())
        return false;
    if (!_SERVER_PORT())
        return false;
    if (!_REQUEST_METHOD())
        return false;
    if (!_PATH_INFO())
        return false;
    if (!_PATH_TRANSLATED())
        return false;
    if (!_SCRIPT_NAME())
        return false;
    if (!_QUERY_STRING())
        return false;
    if (!_QUERY_STRING())
        return false;
    if (!_CONTENT_TYPE())
        return false;
    if (!_CONTENT_LENGTH())
        return false;
    if (!_OtherHeaders())
        return false;
    return (true);
}
bool clsCGI::_SERVER_SOFTWARE()
{
    short Length = HelperFunctions::ft_strlen("SERVER_NAME=FastServer");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "SERVER_NAME=FastServer", SIZE_BUFFER, _Offset,Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}
bool clsCGI::_SERVER_NAME()
{
    short Length = HelperFunctions::ft_strlen("SERVER_SOFTWARE=FastHTTP/1.1");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "SERVER_SOFTWARE=FastHTTP/1.1", SIZE_BUFFER, _Offset,Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}
bool clsCGI::_SERVER_PROTOCOL()
{
    short Length = HelperFunctions::ft_strlen("SERVER_PROTOCOL=HTTP/1.1");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "SERVER_PROTOCOL=HTTP/1.1", SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}
bool clsCGI::_GATEWAY_INTERFACE()
{
    short Length = HelperFunctions::ft_strlen("GATEWAY_INTERFACE=CGI/1.1");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "GATEWAY_INTERFACE=CGI/1.1", SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}
bool clsCGI::_REMOTE_IDENT()
{
    short Length = HelperFunctions::ft_strlen("REMOTE_IDENT=\"\"");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "REMOTE_IDENT=\"\"", SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}
bool clsCGI::_REMOTE_HOST()
{
    short Length = HelperFunctions::ft_strlen("REMOTE_HOST=\"\"");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "REMOTE_HOST=\"\"", SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}
bool clsCGI::_REMOTE_ADDR()
{
    short Length = HelperFunctions::ft_strlen("REMOTE_ADDR=");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "REMOTE_ADDR=", SIZE_BUFFER, _Offset, Length);
    Length = HelperFunctions::ft_strlen(_IPClient);
    HelperFunctions::ft_str_copy(_Buffer, _IPClient, SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_AUTH_TYPE()
{
    short Length = HelperFunctions::ft_strlen("AUTH_TYPE=\"\"");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "AUTH_TYPE=\"\"", SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_REMOTE_USER()
{
    short Length = HelperFunctions::ft_strlen("REMOTE_USER=\"\"");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "REMOTE_USER=\"\"", SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_SERVER_PORT()
{
    short Length = HelperFunctions::ft_strlen("SERVER_PORT=");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    HelperFunctions::ft_str_copy(_Buffer, "SERVER_PORT=", SIZE_BUFFER, _Offset, Length);
    Length = HelperFunctions::ft_strlen(_PortServer);
    HelperFunctions::ft_str_copy(_Buffer, _PortServer, SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_REQUEST_METHOD()
{
    char c_Method[2][8] ={"GET", "POST"};
    u_int8_t Method = 0;
    Method = (_DataRequest.getMethod() != HttpTables::M_GET);
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    short Length = HelperFunctions::ft_strlen("REQUEST_METHOD=");
    HelperFunctions::ft_str_copy(_Buffer, "REQUEST_METHOD=", SIZE_BUFFER, _Offset, Length);
    Length = HelperFunctions::ft_strlen(c_Method[Method]);
    HelperFunctions::ft_str_copy(_Buffer,  c_Method[Method], SIZE_BUFFER, _Offset, Length);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_PATH_INFO()
{
    short Length = 0;
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    if (!_DataRequest.getPathInfo().len)
    {
        Length = HelperFunctions::ft_strlen("PATH_INFO=\"\"");
        HelperFunctions::ft_str_copy(_Buffer, "PATH_INFO=\"\"", SIZE_BUFFER, _Offset, Length);
    }
    else
    {
        Length = HelperFunctions::ft_strlen("PATH_INFO=");
        HelperFunctions::ft_str_copy(_Buffer, "PATH_INFO=", SIZE_BUFFER, _Offset, Length);
        HelperFunctions::ft_str_copy(_Buffer, _DataRequest.getPathInfo().Data, SIZE_BUFFER, _Offset, _DataRequest.getPathInfo().len);
    }
     if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_PATH_TRANSLATED()
{
    short Length = 0;
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    if (_DataRequest.getPathTranslated().empty())
    {
        Length = HelperFunctions::ft_strlen("PATH_TRANSLATED=\"\"");
        HelperFunctions::ft_str_copy(_Buffer,  "PATH_TRANSLATED=\"\"", SIZE_BUFFER, _Offset, Length);
    }
    else
    {
        Length = HelperFunctions::ft_strlen("PATH_TRANSLATED=");
        HelperFunctions::ft_str_copy(_Buffer,  "PATH_TRANSLATED=", SIZE_BUFFER, _Offset, Length);
        HelperFunctions::ft_str_copy(_Buffer,  _DataRequest.getPathTranslated().c_str(), SIZE_BUFFER, _Offset, _DataRequest.getPathTranslated().length());
    }
     if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_SCRIPT_NAME()
{
    short Length = HelperFunctions::ft_strlen("SCRIPT_NAME=");
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
     HelperFunctions::ft_str_copy(_Buffer,  "SCRIPT_NAME=", SIZE_BUFFER, _Offset, Length);
     HelperFunctions::ft_str_copy(_Buffer,  _DataRequest.getScriptName().Data, SIZE_BUFFER, _Offset, _DataRequest.getScriptName().len);
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_QUERY_STRING()
{
    short Length = 0;
     if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    if (!_DataRequest.getQuery().len)
    {
        Length = HelperFunctions::ft_strlen("QUERY_STRING=\"\"");
        HelperFunctions::ft_str_copy(_Buffer,  "QUERY_STRING=\"\"", SIZE_BUFFER, _Offset, Length);
    }
    else
    {
        Length = HelperFunctions::ft_strlen("QUERY_STRING=");
        HelperFunctions::ft_str_copy(_Buffer,  "QUERY_STRING=", SIZE_BUFFER, _Offset, Length);
        HelperFunctions::ft_str_copy(_Buffer,  _DataRequest.getQuery().Data, SIZE_BUFFER, _Offset, _DataRequest.getQuery().len);
    }
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_CONTENT_TYPE()
{
    short Length = 0;
     if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    if (_DataRequest.getHeader().getKnownHeader(HttpTables::H_CONTENT_TYPE)->Hash == -1)
    {
        Length = HelperFunctions::ft_strlen("CONTENT_TYPE=\"\"");
        HelperFunctions::ft_str_copy(_Buffer,  "CONTENT_TYPE=\"\"", SIZE_BUFFER, _Offset, Length);
    }
    else
    {
        Length = HelperFunctions::ft_strlen("CONTENT_TYPE=");
        HelperFunctions::ft_str_copy(_Buffer, "CONTENT_TYPE=", SIZE_BUFFER, _Offset, Length);
        HelperFunctions::ft_str_copy(_Buffer,
             _DataRequest.getHeader().getKnownHeader(HttpTables::H_CONTENT_TYPE)->val.Data, SIZE_BUFFER, _Offset, _DataRequest.getHeader().getKnownHeader(HttpTables::H_CONTENT_TYPE)->val.len);
    }
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_CONTENT_LENGTH()
{
    short Length = 0;
    if (_Offset == SIZE_BUFFER)
        return false;
    _ENV[_Counter] = &_Buffer[_Offset];
    if (_DataRequest.getHeader().getKnownHeader(HttpTables::H_CONTENT_LENGTH)->Hash == -1)
    {
        Length = HelperFunctions::ft_strlen("CONTENT_LENGTH=\"\"");
        HelperFunctions::ft_str_copy(_Buffer,  "CONTENT_LENGTH=\"\"", SIZE_BUFFER, _Offset, Length);
    }
    else
    {
        Length = HelperFunctions::ft_strlen("CONTENT_LENGTH=");
        HelperFunctions::ft_str_copy(_Buffer,  "CONTENT_LENGTH=", SIZE_BUFFER, _Offset, Length);
        HelperFunctions::ft_str_copy(_Buffer,
             _DataRequest.getHeader().getKnownHeader(HttpTables::H_CONTENT_LENGTH)->val.Data, SIZE_BUFFER, _Offset, _DataRequest.getHeader().getKnownHeader(HttpTables::H_CONTENT_LENGTH)->val.len);
    }
    if (_Offset == SIZE_BUFFER)
        return false;
    _Counter++;
    _Offset++;
    return (true);
}

bool clsCGI::_OtherHeaders()
{
    int i = 0;
    while (i < SIZE_VAR_ENV)
    {
        // _DataRequest.getHeader().getUnknownHeader(0)->val.
        
        i++;
    }
    return (true);
}
bool clsCGI::_StoredArgs()
{
    _ARG[0] = (char *)_DataRequest.getPathCgi()->c_str();
    if (!_ARG[0])
        return (false);
    _ARG[1] = _DataRequest.getPhysicalPath();
    if (!_ARG[1])
        return (false);
    _ARG[2] = NULL;
    return (true);
}

bool clsCGI::_childeProcesse()
{
    int Fd = -1;
    close(_pip[0]);
    if (_DataRequest.getMethod() == HttpTables::M_POST)
    {
        Fd = open(_DataRequest.getFilePathBody().c_str(), O_RDONLY | O_CLOEXEC, 0644);
        if (Fd < 0)
        {
            close(_pip[1]);
            return (true);
        }
        if (dup2(Fd, 0) == -1)
            return (close(Fd), close(_pip[1]), true);
    }
    // int Start = HelperFunctions::FindChar(_DataRequest.getPhysicalPath(), HelperFunctions::ft_strlen(_DataRequest.getPhysicalPath()), '.');
    // int Pos = HelperFunctions::FindCharFromLast(_DataRequest.getPhysicalPath(), Start, '/');
    // _DataRequest.getPhysicalPath()[Pos] = '\0';
    // if (chdir(_DataRequest.getPhysicalPath())  == -1)
    //     return (close(Fd), close(_pip[1]), true);
    if (dup2(_pip[1], 1) == -1)
        return (close(Fd), close(_pip[1]), true);
    close(_pip[1]);
    if (_DataRequest.getMethod() == HttpTables::M_POST)
        close(Fd);
    
    execve(_ARG[0], _ARG, _ENV);
    return true;
}

void clsCGI::_ParentProcesse()
{
    close(_pip[1]);
    _IsRunCGI = true;
    _FD = _pip[0];
}

bool clsCGI::_InintialVar()
{
    if (!_MakeEnv())
        return (false);
    if (!_StoredArgs())
        return (false);
    if (pipe(_pip) == -1)
        return (false);
    return (true);
}

void clsCGI::RunCGI()
{
    if (!_InintialVar())
    {
        _Erno = true;
        _FD = -1;
        return;
    }
    _PIDCHILD = fork();
    if (_PIDCHILD < 0)
    {
        close(_pip[0]);
        close(_pip[1]);
        _Erno = true;
        _FD = -1;
        return;
    }
    _StartTime = HelperFunctions::getCurrentTimeInS();
    if (_PIDCHILD == 0)
    {
        if (_childeProcesse())
            exit(1);
    }
    else
        _ParentProcesse();
    return;
}

void clsCGI::Reset()
{
    _IsRunCGI = false;
    _Counter = 0;
    _Offset = 0;
    _Erno = false;
}

bool clsCGI::GetIsRunCGI()
{
    return (_IsRunCGI);
}

void clsCGI::SetIsRunCGI(bool IsRunCGI)
{
    _IsRunCGI = IsRunCGI;
}

clsParseOutCGI &clsCGI::GetclsParseOutCGI()
{
    return _ParseOutCGI;
}
int clsCGI::GetPid()
{
    return _PIDCHILD;
}

const time_t &clsCGI::getStartTime() const
{
    return this->_StartTime;
};

int clsCGI::GetFdPipe()
{
    return _FD;
}
bool clsCGI::GetErno()
{
    return _Erno;
}

void clsCGI::SetBuffer(char *Buffer)
{
    _Buffer = Buffer;
}

void clsCGI::SetPortS_and_IPC(const char *IPC, const char *PosrtS)
{
    _IPClient = IPC;
    _PortServer = PosrtS;
}

clsCGI::~clsCGI()
{
    _pip[0] = -1;
    _pip[1] = -1;
}
