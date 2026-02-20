/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 15:13:46 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/20 21:38:21 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"
using namespace std;
int main(void)
{
    stErrorPagedata ErrorPage;
    ErrorPage.Path = "/mnt/homes/achamdao/Desktop/Dev/PartRespond/response/file.html";
    ErrorPage.Status = -1;
    RequestHandler RequestData;
    RequestData._physicalPath = "/mnt/homes/achamdao/Desktop/Dev/PartRespond/response/file.h";
    RequestData._method = GET;
    RequestData._error_pages[404] = ErrorPage;
    clsMainProcess MainProcess;
    MainProcess.MainProcess(RequestData);
    clsResponse Response = MainProcess.GetclsResponse();
    cout << Response.GetHeaderFeild();
    cout << Response.GetBody();
    cout << Response.GetFileName();
    return 0;
}