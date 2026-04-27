/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:40:27 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/19 21:23:13 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLS_MAIN_PROCESS_HPP
#define CLS_MAIN_PROCESS_HPP

#include "../response/clsResponse.hpp"
#include "../cgi/clsParseOutCGI.hpp"
#include "../cgi/clsCGI.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"

class clsMainProcess
{
private:
    clsResponse _Response;
    clsErrorPage _ErrorPage;
    clsCGI _CGI;
    RequestHandler &_DataRequest;
    bool _RunCGI;
    void _PartRedirection();
    void _PartPermission();
    void _PartDeleteMethod();
    void _PartPOSMethod();
    void _PartGETMethod();
    void _PartErrorRequest();
    void _InitializeCGI();
    stEventProcess::eEventProcess _eventProcess;

public:
    
    void MainProcess();
    void ParseCGI(const char *Buffer, short Length);
    clsMainProcess(RequestHandler &RequestLinker);
    stEventProcess::eEventProcess &getEventProcess();
    void setEventProcess(stEventProcess::eEventProcess ev);
    bool isRunCgi();
    clsCGI &GetclsCGI();
    bool GetIsRunCGI();
    clsResponse &GetclsResponse();
    void Reset();
    ~clsMainProcess();
};
#endif
