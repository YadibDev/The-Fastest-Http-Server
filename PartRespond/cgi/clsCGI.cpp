/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:40:02 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/24 21:54:22 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clsCGI.hpp"

void clsCGI::_StoredWhiteBlakHeaders()
{
    if (_WhiteBlakHeaders.empty())
    {
        _WhiteBlakHeaders["Authorization"] = 0;
        _WhiteBlakHeaders["Proxy-Authorization"] = 0;
        _WhiteBlakHeaders["Connection"] = 0;
        _WhiteBlakHeaders["Keep-Alive"] = 0;
        _WhiteBlakHeaders["Upgrade"] = 0;
        _WhiteBlakHeaders["Te"] = 0;
        _WhiteBlakHeaders["Trailer"] = 0;
        _WhiteBlakHeaders["Transfer-Encoding"] = 0;

        _WhiteBlakHeaders["Auth-Type"] = 1;
        _WhiteBlakHeaders["Remote-Ident"] = 1;
        _WhiteBlakHeaders["Remote-User"] = 1;
        _WhiteBlakHeaders["Content-Length"] = 1;
        _WhiteBlakHeaders["Content-Type"] = 1;
        _WhiteBlakHeaders["Gateway-Interface"] = 1;
        _WhiteBlakHeaders["Path-Info"] = 1;
        _WhiteBlakHeaders["Path-Translated"] = 1;
        _WhiteBlakHeaders["Query-String"] = 1;
        _WhiteBlakHeaders["Script-Name"] = 1;
        _WhiteBlakHeaders["Remote-Addr"] = 1;
        _WhiteBlakHeaders["Remote-Host"] = 1;
        _WhiteBlakHeaders["Request-Method"] = 1;
        _WhiteBlakHeaders["Server-Name"] = 1;
        _WhiteBlakHeaders["Server-Port"] = 1;
        _WhiteBlakHeaders["Server-Protocol"] = 1;
        _WhiteBlakHeaders["Server-Software"] = 1;
    }
}

const std::string  &clsCGI::_ConcatonateValue(const std::vector <std::string> &Value)
{
    std::string EnvValue;
    for (size_t i = 0; i < Value.size(); i++)
    {
        EnvValue += Value[i];
        std::cout << i<< std::endl;
        if (i != Value.size() - 1)
            EnvValue += ";";
    }
    return (EnvValue);
}

std::string clsCGI::_BuildVarEnv(const std::string &HeaderName,const std::string  &Value)
{
    size_t Pos = 0;
    std::string EnvValue;

    if (!_WhiteBlakHeaders.count(HeaderName))
        EnvValue += "HTTP_";
    EnvValue += HeaderName;
    EnvValue = HelperFunctions::ConvertStringToUpper(EnvValue);
    while ((Pos = EnvValue.find('-')) != std::string::npos)
        EnvValue = EnvValue.replace(Pos, 1, "_");
    EnvValue += "=";
    EnvValue += Value;
    return (EnvValue);
}

char **clsCGI::_MakeEnv()
{
    std::map<std::string, std::vector<std::string> >::iterator it;
    std::map<std::string, std::vector<std::string> > Header;
    char **ENV = NULL;
    std::vector<std::string> Variables;

    for (it = Header.begin(); it != Header.end(); it++)
    {
        if (_WhiteBlakHeaders.count(it->first))
            if (!_WhiteBlakHeaders[it->first])
                continue;
        Variables.push_back(_BuildVarEnv(it->first, _ConcatonateValue(it->second)));
    }
    ENV = new char*[Variables.size() + 1];
    if (!ENV)
        return (NULL);
    for (size_t i = 0; i < Variables.size(); i++)
    {
        ENV[i] = HelperFunctions::ft_strdup(Variables[i].c_str());
        if (!ENV[i])
        {
            HelperFunctions::free_matrex(&ENV);
            return (NULL);
        }
    }
    ENV[Variables.size()] = NULL;
    return ENV;
}

char **clsCGI::_StoredArgs()
{
    char **ARG = NULL;
    ARG = new char*[3];
    if (!ARG)
        return (NULL);
    ARG[0] = HelperFunctions::ft_strdup(_DataRequest.getPathCgi().c_str());
    if (!ARG[0])
        return (NULL);
    ARG[1] = HelperFunctions::ft_strdup(_DataRequest.getPhysicalPath().c_str());
    if (!ARG[1])
    {
        HelperFunctions::free_matrex(&ARG);
        return (NULL);
    }
    ARG[2] = NULL;
    return (ARG);
}

int clsCGI::RunCGI()
{
    int status;
    int exit_code;
    char **ENV = NULL;
    char **ARG = NULL;
    int Fd = -2;
    int pip[2] = {-1,-1};
    int pipBody[2] = {-1,-1};
    if (!(ENV = _MakeEnv()))
        return (-500);
    if (!(ARG = _StoredArgs()))
    {
        HelperFunctions::free_matrex(&ENV);
        return (-500);
    }
    if (_DataRequest.getBody() != "")
    {
        if (pipe(pipBody) == -1)
        {
            HelperFunctions::free_matrex(&ENV);
            return (-500);
        }
        write(pipBody[1], &_DataRequest.getBody()[0], _DataRequest.getBody().size());
    }
    if (pipe(pip) == -1)
    {
        HelperFunctions::free_matrex(&ENV);
        return (-500);
    }
    if (_DataRequest.getFilePathBody() != "")
        Fd = open(_DataRequest.getFilePathBody().c_str(), O_RDONLY, 644);
    if (Fd != -2 && Fd < 0)
    {
        close(pip[0]);
        close(pip[1]);
        HelperFunctions::free_matrex(&ENV);
        return (-500);
    }
    _StartTime = HelperFunctions::getCurrentTimeInS();
    _PIDCHILD = fork();
    if (_PIDCHILD < 0)
    {
        close(pip[0]);
        close(pip[1]);
        HelperFunctions::free_matrex(&ENV);
        return (-500);
    }
    if (_PIDCHILD == 0)
    {
        close(pip[0]);
        close(pipBody[1]);
        if (_DataRequest.getFilePathBody() != "")
            if (dup2(Fd, 0) == -1)
                return (close(Fd), close(pip[1]), -1);
        else if (_DataRequest.getBody() != "")
            if (dup2(pipBody[0], 0) == -1)
                return (close(pipBody[0]), close(pip[1]), -1);
        if (dup2(pip[1], 1) == -1)
            return (close(Fd), close(pip[1]), -1);
        close(pip[1]);
        execve(_DataRequest.getPathCgi().c_str(), ARG, ENV);
        return -1;
    }
    else
    {
        close(pip[1]);
        close(pipBody[0]);
        close(pipBody[1]);
        exit_code = waitpid(_PIDCHILD, &status, WNOHANG);
        if (exit_code < 0)
        {
            HelperFunctions::free_matrex(&ENV);
            HelperFunctions::free_matrex(&ARG);
            close(pip[0]);
            close(Fd);
            return (-500);
        }
        else if (WEXITSTATUS(status) == 1)
        {
            HelperFunctions::free_matrex(&ARG);
            HelperFunctions::free_matrex(&ENV);
            close(pip[0]);
            close(Fd);
            return (-500);
        }
        else if (exit_code > 0)
        {
            HelperFunctions::free_matrex(&ARG);
            HelperFunctions::free_matrex(&ENV);
            close(Fd);
            return (-500);
            return (pip[0]);
        }
    }
    return 0;
}
