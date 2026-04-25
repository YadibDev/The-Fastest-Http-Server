/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yadib <yadib@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:40:02 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/25 14:48:46 by yadib            ###   ########.fr       */
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
    _pip[0] = -1;
    _pip[1] = -1;
    TempVar.resize(MAX_HEADERS);
    if (TempVar.empty())
    {
        _Erno = true;
        return ;
    }
    TempVar.clear();
}

bool clsCGI::_MakeEnv()
{
    _ENV = new(std::nothrow) char*[SIZE_VAR_ENV]; // 
    if (!_ENV)
        return (false);
    HelperFunctions::ft_memset(_ENV,0,(sizeof(_ENV) * SIZE_VAR_ENV));
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
    if (!(_ENV[_Counter] = HelperFunctions::GetENV_VAR_CONST(0)))
    {
        return (false);
    }
    _Counter++;
    return (true);
}
bool clsCGI::_SERVER_NAME()
{
     if (!(_ENV[_Counter] = HelperFunctions::GetENV_VAR_CONST(1)))
     {
        return (false);
     }
    _Counter++;
    return (true);
}
bool clsCGI::_SERVER_PROTOCOL()
{
    if (!(_ENV[_Counter] = HelperFunctions::GetENV_VAR_CONST(2)))
    {

        return (false);
    }
    _Counter++;
    return (true);
}
bool clsCGI::_GATEWAY_INTERFACE()
{
    if (!(_ENV[_Counter] =HelperFunctions::GetENV_VAR_CONST(3)))
    {

        return (false);
    }
    _Counter++;
    return (true);
}
bool clsCGI::_REMOTE_IDENT()
{
    if (!(_ENV[_Counter] = HelperFunctions::GetENV_VAR_CONST(4)))
    {

        return (false);
    }
    _Counter++;
    return (true);
}
bool clsCGI::_REMOTE_HOST()
{
    if (!(_ENV[_Counter] = HelperFunctions::GetENV_VAR_CONST(5)))
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
        return (false);
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
    _ARG[1] = HelperFunctions::ft_strdup("/home/yadib/goinfre/The-Fastest-Http-Server/cgi-bin/upload.php");
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
    //     close(_pip[1]);
    //     HelperFunctions::free_matrex(&_ENV);
    //     std::cout << "lkapo\n";
    //     return (-500);
    // }
    // if (dup2(Fd, 0) == -1)
    //     return (close(Fd), close(_pip[1]), true);
    if (dup2(_pip[1], 1) == -1)
        return (close(Fd), close(_pip[1]), true);
    close(_pip[1]);
    // close(Fd);
    execve(_ARG[0], _ARG, _ENV);
    return true;
}

void clsCGI::_ParentProcesse()
{
    // stEventProcess::eEventProcess exit_code = HelperFunctions::checkProcessStatus(_PIDCHILD);
    // close(_pip[1]);
    // if (exit_code == stEventProcess::END_UNKNOW)
    // {
    //     close(_pip[0]);
    //     _FD = -1;
    //     _Erno = true;
    // }
    // else
    // {
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
        return ;
    }
    _PIDCHILD = fork();
    if (_PIDCHILD < 0)
    {
        close(_pip[0]);
        close(_pip[1]);
       _Erno = true;
       _FD = -1;
       return ;
    }
    _StartTime = HelperFunctions::getCurrentTimeInS();
    if (_PIDCHILD == 0)
    {
        if (_childeProcesse())
            exit(1);
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
clsCGI::~clsCGI()
{
    HelperFunctions::free_matrex(&_ENV);
    HelperFunctions::free_matrex(&_ARG);
    kill(_PIDCHILD,SIGKILL);
    close(_pip[0]);
    _pip[0] = -1;
    _pip[1] = -1;
}

