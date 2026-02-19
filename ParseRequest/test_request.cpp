#include <iostream>
#include <string>
#include "Request.hpp"

int main() {
    clsRequest req;
    std::string raw = "GET /index.html?name=abc HTTP/1.1\r\nHost: example.com\r\nContent-Length: 0\r\n\r\n";
    req.parse(raw);
    std::cout << "parse() called. Completed: " << (req.isCompleted() ? "yes" : "no") << std::endl;
    return 0;
}
