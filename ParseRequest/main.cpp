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



#include <iostream>
#include <string>
#include <iomanip>

#define RESET   "\033[0m"
#define RED     "\033[31m"      // للخطأ
#define GREEN   "\033[32m"      // للنجاح
#define YELLOW  "\033[33m"      // للعناوين
#define BLUE    "\033[34m"      // للمدخلات
#define CYAN    "\033[36m"      // للقيم المستخرجة


#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>


struct s_ListenAddress {
    struct sockaddr_storage storage;
    int family;
    socklen_t len;
};


int getIPTypeSimple(const std::string& ip) {
    if (ip.empty()) return 0;

    bool hasDot = (ip.find('.') != std::string::npos);
    bool hasColon = (ip.find(':') != std::string::npos);

    if (hasColon && !hasDot) return 6;
    if (hasDot && !hasColon) return 4;

    return 0;
}


void    extractClientMaxBodySize(std::string str, size_t &result)
{
    if (str.empty()) throw 400;

    result = 0;
    size_t i = 0;

    while (i < str.size() && isdigit(str[i]))
    {
        size_t prev = result;
        result = (result * 10) + (str[i] - '0');
        if (result < prev) throw 400;
        i++;
    }

    if (i < str.size())
    {
        char unit = toupper(str[i]);
        if (unit == 'K') result *= 1024;
        else if (unit == 'M') result *= 1024 * 1024;
        else if (unit == 'G') result *= 1024 * 1024 * 1024;
        else throw 400;
        
        i++;
    }
}

int main() {
    std::string listen = "";
    unsigned short port = 0;
    std::string ip = "";
    uint32_t IPv4 = 0;

    std::cout << YELLOW << "========================================" << RESET << std::endl;
    std::cout << YELLOW << "     Webserv URI/Listen Manual Tester   " << RESET << std::endl;
    std::cout << YELLOW << "========================================" << RESET << std::endl;
    std::cout << "Examples: [::1]:80, 127.0.0.1:443, *, 8080" << std::endl;

    while (true)
    {
        std::cout << BLUE << "\n[?] Enter Listen string: " << RESET;
        if (!std::getline(std::cin, listen)) {
            std::cout << "\nExiting..." << std::endl;
            break;
        }

        port = 0;
        ip = "";

        try {
            std::cout << GREEN << "[+] PARSE SUCCESS" << RESET << std::endl;
            std::cout << CYAN << "    " << std::setw(10) << std::left << "IP Address : " << RESET << ip << std::endl;
            std::cout << CYAN << "    " << std::setw(10) << std::left << "    IP Address Binary: " << RESET << IPv4 << std::endl;
            std::cout << std::dec << std::endl;            std::cout << CYAN << "    " << std::setw(10) << std::left << "Port       : " << RESET << port << std::endl;
        } 
        catch (int error) {
            std::cerr << RED << "[!] PARSE FAILED" << RESET << std::endl;
            std::cerr << RED << "    Error Code: " << error << RESET << std::endl;
            std::cerr << RED << "    Reason    : Bad Request / Invalid Format" << RESET << std::endl;
        }

        std::cout << YELLOW << "----------------------------------------" << RESET << std::endl;
        listen.clear();
    }

    return 0;
}






