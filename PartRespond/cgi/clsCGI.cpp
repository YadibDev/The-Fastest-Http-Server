/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:40:02 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/19 21:16:54 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsCGI.hpp"
short clsCGI::_LimitProcess;
clsCGI::clsCGI(const RequestHandler &DataRequest) : _DataRequest(DataRequest), _ParseOutCGI(DataRequest)
{
    _IsRunCGI = false;
    _ARG = NULL;
    _ENV = NULL;
    _Counter = 0;
    _Erno = false;
    TempVar.resize(2000);
    if (TempVar.empty())
    {
        _Erno = true;
        return ;
    }
    TempVar.clear();
}

bool clsCGI::_MakeEnv()
{
    _ENV = new char*[SIZE_VAR_ENV];
    if (!_ENV)
        return (false);
    HelperFunctions::ft_memset(_ENV,0,sizeof(_ENV) * SIZE_VAR_ENV);
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
    if (_PATH_TRANSLATED())
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
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("SERVER_SOFTWARE=FastHTTP/1.1")))
    {
        return (false);
    }
    _Counter++;
    return (true);
}
bool clsCGI::_SERVER_NAME()
{
     if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("SERVER_NAME=FastServer")))
     {

        return (false);
     }
    _Counter++;
    return (true);
}
bool clsCGI::_SERVER_PROTOCOL()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("SERVER_PROTOCOL=HTTP/1.1")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}
bool clsCGI::_GATEWAY_INTERFACE()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("GATEWAY_INTERFACE=CGI/1.1")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}
bool clsCGI::_REMOTE_IDENT()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("REMOTE_IDENT=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}
bool clsCGI::_REMOTE_ADDR()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("REMOTE_ADDR=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_AUTH_TYPE()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("AUTH_TYPE=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_REMOTE_USER()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("REMOTE_USER=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_SERVER_PORT()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("SERVER_PORT=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_REQUEST_METHOD()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("REQUEST_METHOD=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_PATH_INFO()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("PATH_INFO=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_PATH_TRANSLATED()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("PATH_TRANSLATED=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_SCRIPT_NAME()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("SCRIPT_NAME=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_QUERY_STRING()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("QUERY_STRING=\"\"")))
    {

        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_CONTENT_TYPE()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("CONTENT_TYPE=\"\"")))
    {
        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_CONTENT_LENGTH()
{
    if (!(_ENV[_Counter] = HelperFunctions::ft_strdup("CONTENT_LENGTH=\"\"")))
    {
        return (false);
    }
    _Counter++;
    return (true);
}

bool clsCGI::_OtherHeaders()
{
    
    return (true);
}
bool clsCGI::_StoredArgs()
{
    _ARG = new(std::nothrow) char*[3];
    if (!_ARG)
        return (false);
    _ARG[0] = HelperFunctions::ft_strdup(_DataRequest.getPathCgi()->c_str());
    if (!_ARG[0])
        return (false);
    _ARG[1] = HelperFunctions::ft_strdup(_DataRequest.getPhysicalPath());
    if (!_ARG[1])
        return (false);
    _ARG[2] = NULL;
    return (true);
}

bool clsCGI::_childeProcesse()
{
    int Fd = -1;
    close(_pip[0]);
    // Fd = open("_DataRequest.getFilePathBody().c_str()", O_RDONLY, 644);
    // if (Fd < 0)
    // {
    //     close(pip[1]);
    //     HelperFunctions::free_matrex(&ENV);
    //     std::cout << "lkapo\n";
    //     return (-500);
    // }
    // if (dup2(Fd, 0) == -1)
    //     return (close(Fd), close(pip[1]), true);
    if (dup2(_pip[1], 1) == -1)
        return (close(Fd), close(_pip[1]), true);
    close(_pip[1]);
    close(Fd);
    execve(_ARG[0], _ARG, _ENV);
    return true;
}

void clsCGI::_ParentProcesse()
{
    int status;
    int exit_code;
    close(_pip[1]);
    exit_code = waitpid(_PIDCHILD, &status, WNOHANG);
    if (exit_code < 0)
    {
        close(_pip[0]);
        _FD = -1;
        _Erno = true;
    }
    else if (WEXITSTATUS(status) == 1)
    {
        close(_pip[0]);
        _Erno = true;
       _FD = -1;
    }
    else if (exit_code > 0)
    {
        _IsRunCGI = true;
        _FD = _pip[0];
        return ;
    }
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
    }
    _PIDCHILD = fork();
    if (_PIDCHILD < 0)
    {
        close(_pip[0]);
        close(_pip[1]);
       _Erno = true;
       _FD = -1;
    }
    _StartTime = HelperFunctions::getCurrentTimeInS();
    if (_PIDCHILD == 0)
    {
        if (_childeProcesse())
            exit(1) ;
    }
    else
       _ParentProcesse();
    return ;
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

int clsCGI::GetFdPipe()
{
    return _FD;
}
bool clsCGI::GetErno()
{
    return _Erno;
}
clsCGI::~clsCGI()
{
   
    HelperFunctions::free_matrex(&_ENV);
    HelperFunctions::free_matrex(&_ARG);
    kill(_PIDCHILD,SIGKILL);
    close(_pip[0]);
}

