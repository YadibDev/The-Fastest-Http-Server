/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:40:02 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/13 15:43:30 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsCGI.hpp"

clsCGI::clsCGI()
{
    _StoredWhiteBlakHeaders();
    _IsRunCGI = false;
}

void clsCGI::_StoredWhiteBlakHeaders()
{
    if (_WhiteBlakHeaders.empty())
    {
        _WhiteBlakHeaders["authorization"] = 0;
        _WhiteBlakHeaders["proxy-Authorization"] = 0;
        _WhiteBlakHeaders["connection"] = 0;
        _WhiteBlakHeaders["keep-alive"] = 0;
        _WhiteBlakHeaders["upgrade"] = 0;
        _WhiteBlakHeaders["te"] = 0;
        _WhiteBlakHeaders["trailer"] = 0;
        _WhiteBlakHeaders["transfer-Encoding"] = 0;
    }
}

// void clsCGI::_BuildBasicVar(std::vector <std::string> &Var)
// {
//     std::map<std::string, std::vector<std::string> > Header;
//     if (Header.count("authorization"))
//         Var.push_back(_BuildVarEnv("AUTH_TYPE", _ConcatonateValue(Header["Authorization"])));
//     if (Header.count("content-length"))
//         Var.push_back(_BuildVarEnv("CONTENT_LENGTH", _ConcatonateValue(Header["content-length"])));
//     if (Header.count("content-type"))
//         Var.push_back(_BuildVarEnv("CONTENT_TYPE", _ConcatonateValue(Header["content-type"])));
//     Var.push_back(_BuildVarEnv("GATEWAY_INTERFACE", "CGI/1.1"));
//     Var.push_back(_BuildVarEnv("SERVER_SOFTWARE", "FastServer"));
//     Var.push_back(_BuildVarEnv("SERVER_PROTOCOL", "HTTP/1.1"));
//     Var.push_back(_BuildVarEnv("SERVER_NAME", "Server/1337"));
//     Var.push_back(_BuildVarEnv("REQUEST_METHOD", _DataRequest.getMethod()));
//     Var.push_back(_BuildVarEnv("QUERY_STRING", _DataRequest.getQuery()));
//     Var.push_back(_BuildVarEnv("REMOTE_IDENT", ""));
//     Var.push_back(_BuildVarEnv("REMOTE_HOST", ""));
//     // Var.push_back(_BuildVarEnv("PATH_INFO", _DataRequest.));
//     // Var.push_back(_BuildVarEnv("SERVER_PORT", _DataRequest.()));
//     // Var.push_back(_BuildVarEnv("REMOTE_ADDR", give this from socket IP));
// }

// const std::string  &clsCGI::_ConcatonateValue(const std::vector <std::string> &Value)
// {
//     std::string EnvValue;
//     for (size_t i = 0; i < Value.size(); i++)
//     {
//         EnvValue += Value[i];
//         std::cout << i<< std::endl;
//         if (i != Value.size() - 1)
//             EnvValue += ";";
//     }
//     return (EnvValue);
// }

// std::string clsCGI::_BuildVarEnv(const std::string &HeaderName,const std::string  &Value)
// {
//     size_t Pos = 0;
//     std::string EnvValue;

//     if (!_WhiteBlakHeaders.count(HeaderName))
//         EnvValue += "HTTP_";
//     EnvValue += HeaderName;
//     EnvValue = HelperFunctions::ConvertStringToUpper(EnvValue);
//     while ((Pos = EnvValue.find('-')) != std::string::npos)
//         EnvValue = EnvValue.replace(Pos, 1, "_");
//     EnvValue += "=";
//     EnvValue += Value;
//     return (EnvValue);
// }

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
    int Fd;
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
    execve("/usr/bin/php", ARG, ENV);
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
        return (-1);
    }
    _IsRunCGI = true;
    HelperFunctions::free_matrex(&ARG);
    return (pip[0]);
}
bool clsCGI::_InintialVar(char **ENV, char **ARG, int pip[2])
{
    // if (!(ENV = _MakeEnv()))
    //     return (true);
    if (!(ARG = _StoredArgs()))
        return (true);
    if (pipe(pip) == -1)
        return (true);
    _StartTime = HelperFunctions::getCurrentTimeInS();
    return (false);
}
int clsCGI::RunCGI()
{
    char **ENV = NULL;
    char **ARG = NULL;
    int Fd = -1;
    int pip[2] = {-1,-1};
    if (_InintialVar(ENV, ARG,pip))
        return (-1);
    _PIDCHILD = fork();
    if (_PIDCHILD < 0)
    {
        close(pip[0]);
        close(pip[1]);
        HelperFunctions::free_matrex(&ENV);
        return (-1);
    }
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

clsCGI::~clsCGI(){}

