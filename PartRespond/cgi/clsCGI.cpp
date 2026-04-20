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

clsCGI::clsCGI(const RequestHandler &DataRequest) : _DataRequest(DataRequest), _ParseOutCGI(DataRequest)
{
    _IsRunCGI = false;
    _ARG = NULL;
    _ENV = NULL;
}

bool clsCGI::_MakeEnv()
{
    _ENV = new char*[17 + 31 + 1];
    if (!_ENV)
        return (false);
    if (!(_ENV[0] = HelperFunctions::ft_strdup("SERVER_SOFTWARE=FastHTTP/1.1")))
        return (false);
    if (!(_ENV[1] = HelperFunctions::ft_strdup("SERVER_NAME=FastServer")))
        return (false);
    if (!(_ENV[2] = HelperFunctions::ft_strdup("SERVER_PROTOCOL=HTTP/1.1")))
        return (false);
    if (!(_ENV[3] = HelperFunctions::ft_strdup("GATEWAY_INTERFACE=CGI/1.1")))
        return (false);
    if (!(_ENV[4] = HelperFunctions::ft_strdup("REMOTE_IDENT=\"\"")))
        return (false);
    if (!(_ENV[5] = HelperFunctions::ft_strdup("REMOTE_HOST=\"\"")))
        return (false);
    if (!(_ENV[6] = HelperFunctions::ft_strdup(" ")))
        return (false);
    if (!(_ENV[7] = HelperFunctions::ft_strdup(" ")))
        return (false);
    if (!(_ENV[8] = HelperFunctions::ft_strdup(" ")))
        return (false);
    _ENV[17 + 31 + 1] = NULL;
    return (true);
}

bool clsCGI::_StoredArgs()
{
    _ARG = new(std::nothrow) char*[3];
    if (!_ARG)
        return (false);
    _ARG[0] = HelperFunctions::ft_strdup("/usr/bin/php");
    if (!_ARG[0])
        return (false);
    _ARG[1] = HelperFunctions::ft_strdup("file.php");
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

int clsCGI::_ParentProcesse()
{
    int status;
    int exit_code;
    close(_pip[1]);
    exit_code = waitpid(_PIDCHILD, &status, WNOHANG);
    if (exit_code < 0)
    {
        close(_pip[0]);
        HelperFunctions::free_matrex(&_ARG);
        HelperFunctions::free_matrex(&_ENV);
        return (-1);
    }
    else if (WEXITSTATUS(status) == 1)
    {
        close(_pip[0]);
        HelperFunctions::free_matrex(&_ARG);
        HelperFunctions::free_matrex(&_ENV);
        return (-1);
    }
    else if (exit_code > 0)
    {
        _IsRunCGI = true;
        HelperFunctions::free_matrex(&_ARG);
        HelperFunctions::free_matrex(&_ENV);
        return (_pip[0]);
    }
    _IsRunCGI = true;
    HelperFunctions::free_matrex(&_ARG);
    HelperFunctions::free_matrex(&_ENV);
    return (_pip[0]);
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
int clsCGI::RunCGI()
{
    if (!_InintialVar())
        return (-1);
    _PIDCHILD = fork();
    if (_PIDCHILD < 0)
    {
        close(_pip[0]);
        close(_pip[1]);
        HelperFunctions::free_matrex(&_ENV);
        return (-1);
    }
    _StartTime = HelperFunctions::getCurrentTimeInS();
    if (_PIDCHILD == 0)
    {
        if (_childeProcesse())
            return (-1);
    }
    else
        return (_ParentProcesse());
    return 0;
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

clsCGI::~clsCGI(){}

