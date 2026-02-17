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

long long clsCGI::GetCurrentTime() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int clsCGI::RunCGI(std::string NameFile, Data OData, int TimeOut)
{
    int status;
    int exit_code;
    int Fd = -1;
    int pip[2] = {-1,-1};
    if (pipe(pip) == -1)
        return -500;
    Fd = open(NameFile.c_str(), O_RDONLY, 644);
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
        if (chdir(Data.GetFolder()) < 0)
            return (-1);
        execve(Data.GetPath(),Data.GetArgs(), Data.GetEnv());
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