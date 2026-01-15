#include "clsStartLine.hpp"
#include <iostream>
#include <iomanip>
#include "URIParser.hpp"

void printStartLineInfo(const clsStartLine& sl) {
	std::cout << "--- HTTP Start Line Parsing Results ---" << std::endl;
	
	std::cout << std::left << std::setw(15) << "Method:";
	if (sl.getMethod() == clsStartLine::GET) std::cout << "GET";
	else if (sl.getMethod() == clsStartLine::POST) std::cout << "POST";
	else if (sl.getMethod() == clsStartLine::DELETE) std::cout << "DELETE";
	std::cout << std::endl;

	std::cout << std::left << std::setw(15) << "Path:" << sl.getPath() << std::endl;
	std::cout << std::left << std::setw(15) << "Query:" << sl.getQuery() << std::endl;
	std::cout << std::left << std::setw(15) << "Version:" << sl.getVersion() << std::endl;
	std::cout << std::left << std::setw(15) << "Status Code:" << sl.getStatusCode() << std::endl;
	std::cout << "---------------------------------------" << std::endl;
}

#include "URI.hpp"


bool isNumber(const std::string& s) {
	if (s.empty()) return false;

	for (size_t i = 0; i < s.size(); i++) {
		if (isdigit(s[i]) == 0) {
			return false;
		}
	}
	return true;
}




int main() {
	// std::string rawRequest = "GET /index.html?lang=fr HTTP/1.1\r\n";
	std::string listen = "";
	short port = -1;
	std::string ip = "";

	while (1)
	{
		std::cout << "Entre Listen = \n";
		if (!getline(std::cin, listen))
			break;
		std::cout << "getline : " << listen << std::endl;
		port = URIParser::extractPort(listen);
		ip = URIParser::extractHost(listen);
		if (ip != "" && port != -1)
		{
			std::cout << "Port : " << port << std::endl;
			std::cout << "Ip : " << ip << std::endl;
		}
		else if (isNumber(ip))
			std::cout << "Port : " << ip << std::endl;
		else
			std::cout << "Ip : " << ip << std::endl;
		std::cout << "\n\n";
	}

	return 0;
}









bool    parseListen(std::string Listen, unsigned short &Port, std::string &Ip)
{
	if (URIParser::extractPort(Listen, Port).getCodeStatus() != 200)
		// throw (400);
	if (URIParser::extractHost(Listen, Ip).getCodeStatus() != 200)
		// throw (400);
	if (Ip != "")
	{
		if (Ip == "*")
			Ip = "0.0.0.0";
		else if (!URIParser::isValidIPv4(Ip))
		{
			if (!URIParser::isValidIPv6(Ip))
			{
				// status.setStatus(1, "ERROR Ip\n");
				return false;
			}
		}
	}
	else
		Ip = "0.0.0.0";
	if (Port == -1)
	{
		// status.setStatus(1, "ERROR Port\n");
		return false;
	}
}