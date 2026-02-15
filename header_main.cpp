#include <iostream>
#include <map>
#include "Header.hpp"

int main() {
    std::map<std::string, std::string> headerMap;
    stArguments args;
    args._Data = "Host: example.com\r\nUser-Agent: TestAgent\r\nAccept: text/html\r\n\r\n";
    args._Pos = 0;
    args._Error.setStatus(0, "");
    bool TheStartOfHeader = true;

    ParseHeader parser(headerMap);
    if (parser.parseHeader(args, TheStartOfHeader)) {
        for (std::map<std::string, std::string>::iterator it = headerMap.begin();
             it != headerMap.end(); ++it)
            std::cout << it->first << ": " << it->second << std::endl;
    } else {
        std::cerr << "Error parsing headers: " << args._Error.getCodeStatus()
                  << " - " << args._Error.getMsgError() << std::endl;
    }
    return 0;
}
