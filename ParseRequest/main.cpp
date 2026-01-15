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

int main() {
    // std::string rawRequest = "GET /index.html?lang=fr HTTP/1.1\r\n";
    std::cout << "Host = " << URIParser::extractHost("8080") << std::endl;
    std::cout << "Port = " << URIParser::extractPort("Host:8080") << std::endl;

    return 0;
}

