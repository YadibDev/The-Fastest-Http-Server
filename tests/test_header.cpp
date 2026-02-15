#include "../Header.hpp"
#include <cassert>
#include <iostream>

int main() {
    std::map<std::string, std::string> headerMap;
    stArguments args;

    /* simple headers */
    args._Data = "Host: example.com\r\nUser-Agent: TestAgent\r\nAccept: text/html\r\n\r\n";
    args._Pos = 0;
    args._Error.setStatus(0, "");
    bool TheStartOfHeader = true;

    ParseHeader parser(headerMap);
    bool ok = parser.parseHeader(args, TheStartOfHeader);

    assert(ok && "Parsing failed");
    assert(headerMap["Host"] == "example.com");
    assert(headerMap["User-Agent"] == "TestAgent");
    assert(headerMap["Accept"] == "text/html");

    /* folded header value (continuation line) */
    headerMap.clear();
    args._Data = "X-Test: first line\r\n\tsecond line\r\n\r\n";
    args._Pos = 0;
    args._Error.setStatus(0, "");
    TheStartOfHeader = true;
    ok = parser.parseHeader(args, TheStartOfHeader);
    assert(ok);
    // normalized linear whitespace should combine lines with a single space
    assert(headerMap["X-Test"] == "first line second line");

    std::cout << "ParseHeader unit tests passed" << std::endl;
    return 0;
}
