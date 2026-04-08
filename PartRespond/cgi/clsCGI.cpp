/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:40:02 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/04 15:36:34 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsCGI.hpp"

clsCGI::clsCGI()
{
    _IsRunCGI = false;
}

char **clsCGI::_MakeEnv()
{
    // std::map<std::string, std::vector<std::string> >::iterator it;
    // std::map<std::string, std::vector<std::string> > Header;
    // char **ENV = NULL;
    // std::vector<std::string> Variables;

    // _BuildBasicVar(Variables);
    // for (it = Header.begin(); it != Header.end(); it++)
    // {
    //     if (_WhiteBlakHeaders.count(it->first))
    //         if (!_WhiteBlakHeaders[it->first])
    //             continue;
    //     Variables.push_back(_BuildVarEnv(it->first, _ConcatonateValue(it->second)));
    // }
    // ENV = new char*[Variables.size() + 1];
    // if (!ENV)
    //     return (NULL);
    // for (size_t i = 0; i < Variables.size(); i++)
    // {
    //     ENV[i] = HelperFunctions::ft_strdup(Variables[i].c_str());
    //     if (!ENV[i])
    //     {
    //         HelperFunctions::free_matrex(&ENV);
    //         return (NULL);
    //     }
    // }
    // ENV[Variables.size()] = NULL;
    // return ENV;
    return 0;
}

char **clsCGI::_StoredArgs()
{
    char **ARG = NULL;
    ARG = new(std::nothrow) char*[3];
    if (!ARG)
        return (NULL);
    ARG[0] = HelperFunctions::ft_strdup("/usr/bin/php");
    if (!ARG[0])
        return (NULL);
    ARG[1] = HelperFunctions::ft_strdup("file.php");
    if (!ARG[1])
    {
        HelperFunctions::free_matrex(&ARG);
        return (NULL);
    }
    ARG[2] = NULL;
    return (ARG);
}

bool clsCGI::_childeProcesse(char **ENV, char **ARG, int pip[2])
{
    int Fd = -1;
    close(pip[0]);
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
    if (dup2(pip[1], 1) == -1)
        return (close(Fd), close(pip[1]), true);
    
    close(pip[1]);
    close(Fd);
    execve("/usr/bin/php", ARG, NULL);
    return true;
}

int clsCGI::_ParentProcesse(char **ENV, char **ARG, int pip[2])
{
    int status;
    int exit_code;
    close(pip[1]);
    exit_code = waitpid(_PIDCHILD, &status, WNOHANG);
    if (exit_code < 0)
    {
        HelperFunctions::free_matrex(&ENV);
        close(pip[0]);
        return (-1);
    }
    else if (WEXITSTATUS(status) == 1)
    {
        HelperFunctions::free_matrex(&ARG);
        close(pip[0]);
        return (-1);
    }
    else if (exit_code > 0)
    {
        HelperFunctions::free_matrex(&ARG);
        return (pip[0]);
    }
    _IsRunCGI = true;
    HelperFunctions::free_matrex(&ARG);
    return (pip[0]);
}
bool clsCGI::_InintialVar(char **ENV, char ***ARG, int pip[2])
{
    // if (!(ENV = _MakeEnv()))
    //     return (true);
    if (!((*ARG) = _StoredArgs()))
        return (true);
    if (pipe(pip) == -1)
        return (true);
    return (false);
}
int clsCGI::RunCGI()
{
    char **ENV = NULL;
    char **ARG = NULL;
    int Fd = -1;
    int pip[2] = {-1,-1};
    if (_InintialVar(ENV, &ARG, pip))
        return (-1);
    _PIDCHILD = fork();
    if (_PIDCHILD < 0)
    {
        close(pip[0]);
        close(pip[1]);
        HelperFunctions::free_matrex(&ENV);
        return (-1);
    }
    _StartTime = HelperFunctions::getCurrentTimeInS();
    if (_PIDCHILD == 0)
    {
        if (_childeProcesse(ENV, ARG, pip))
            return (-1);
    }
    else
        return (_ParentProcesse(ENV, ARG, pip));
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

