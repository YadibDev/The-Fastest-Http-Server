#include <iostream>
#include <map>
#include "Header.hpp"
using namespace std;

int main() {
    std::map<std::string, std::vector<std::string> > headerMap;
    stArguments args;
    args._Data = "Complex-Fold: test, hi, there\r\n"
        "Next-Header: value\r\n"
        "";
    args._Pos = 0;
    args._Error.setStatus(0, "");
    bool TheStartOfHeader = true;


    std::cout << "\n\n\n";
    ParseHeader parser(headerMap);
    parser.parseHeader(args, TheStartOfHeader);
    for (const auto& header : headerMap) {
        std::cout << "Header: " << header.first << "\n";
        for (const auto& value : header.second) {
            std::cout << "  Value: " << value << "\n";
        }
    }
    std::cout << "ERROR CODE: " << args._Error.getCodeStatus() << " - " << args._Error.getMsgError() << std::endl;
    return 0;
}
