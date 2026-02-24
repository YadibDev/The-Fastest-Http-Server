/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:40:02 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/10 18:45:35 by achamdao         ###   ########.fr       */
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
        _WhiteBlakHeaders["Path-Info"] = "PATH";
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

std::string clsCGI::_BuildVarEnv(std::string &HeaderName,const std::vector<std::string>  &Value)
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
    for (int i = 0; i < Value.size(); i++)
    {
        EnvValue += Value[i];
        if (i != Value.size())
            EnvValue += ";";
    }
    return (EnvValue);
}

char **clsCGI::MakeEnv()
{
    size_t Pos = 0;
    std::string HeaderName = "Content-Type";
    std::string EnvValue;
    std::vector<std::string >  Value;
}



int clsCGI::RunCGI()
{
    int status;
    int exit_code;
    int Fd = -1;
    int pip[2] = {-1,-1};
    if (pipe(pip) == -1)
        return -500;
    Fd = open(_DataRequest.getFilePathBody().c_str(), O_RDONLY, 644);
    if (Fd < 0)
        return -500;
    _StartTime = GetCurrentTime();
    _PIDCHILD = fork();
    if (_PIDCHILD < 0)
        return (close(Fd), -500);
    if (_PIDCHILD == 0)
    {
        close(pip[0]);
        if (dup2(pip[1], 1) == -1)
            return (close(Fd), -1);
        close(pip[1]);
        // execve(_DataRequest.getPathCgi().c_str(),_DataRequest., Data.GetEnv());
        return -1;
    }
    else
    {
        close(pip[1]);
        exit_code = waitpid(_PIDCHILD, &status, WNOHANG);
        if (exit_code < 0)
            return (close(pip[0]), close(Fd),-500);
        else if (WEXITSTATUS(status) == 1)
            return (close(pip[0]),close(Fd), -500);
        else if (exit_code > 0)
            return (close(Fd), pip[0]);
    }
    return 0;
}