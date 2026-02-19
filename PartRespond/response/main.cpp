/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 15:13:46 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/19 15:23:16 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../mainprocess/Webserv.hpp"
using namespace std;
int main(void)
{
    clsResponse Response;
    Response.SetFileFromDisk("file.txt");
    Response.SetStatus(200);
    Response.SetType(GetTypeDataFile("response/file"));
    Response.SetMod(GET);
    cout << Response.MakeResponse();
    cout << Response.GetBody();
    cout << Response.GetFileName();
    return 0;
}