#include <iostream>
#include <string>
#include <vector>
#include "Request.hpp"

void debug_req(const std::string& label, const clsRequest& req) {
    std::cout << "--- " << label << " ---" << std::endl;
    std::cout << "Status:  " << (req.isCompleted() ? "✅ COMPLETED" : "⏳ IN PROGRESS") << std::endl;

    if (req.isCompleted()) {
        std::cout << "Method:  [" << req._startLine.getMethod() << "]" << std::endl;
        std::cout << "Path:    [" << req._startLine.getPath() << "]" << std::endl;
        std::cout << "Version: [" << req._startLine.getVersion() << "]" << std::endl;

        // طباعة الترويسات باستخدام getHeaderValues لضمان كبسلة البيانات (Encapsulation)
        //
        
        std::cout << "Headers Detail:" << std::endl;

        // اختبار ترويسة Host
        std::vector<std::string> host = req._headerParser.getHeaderValues("Host");
        if (!host.empty()) std::cout << "  Host: " << host[0] << std::endl;

        // اختبار ترويسة User-Agent
        std::vector<std::string> ua = req._headerParser.getHeaderValues("User-Agent");
        if (!ua.empty()) std::cout << "  User-Agent: " << ua[0] << std::endl;

        // اختبار ترويسة Accept (التي غالباً ما تحتوي على قيم متعددة)
        //
        std::vector<std::string> accept = req._headerParser.getHeaderValues("Content-Length");
        if (!accept.empty()) {
            std::cout << "  Content-Length: ";
            for (size_t i = 0; i < accept.size(); ++i) {
                std::cout << "[" << accept[i] << "]" << (i < accept.size() - 1 ? ", " : "");
            }
            std::cout << std::endl;
        }
    }
    std::cout << "----------------------------" << std::endl;
}
int main() {
    clsRequest req1;
    
    std::string pipelineRequest = 
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length : 5\r\n"
        "\r\n";
    std::cout << "🔥 Starting 1-Byte Pipelining Test..." << std::endl;

    for (size_t i = 0; i < pipelineRequest.length(); ++i) {
        std::string singleByte(1, pipelineRequest[i]);
        req1.parse(singleByte); 

        if (req1.isCompleted()) {
            debug_req("Request 1 Finished", req1);
            break; 
        }
    }

    return 0;
}