#include <iostream>
#include <string>

struct RequestLine {
    std::string method;
    std::string uri;
    std::string version;
};

RequestLine parse_request_line(const std::string& request_data) {
    RequestLine req;

    // نبحث عن المسافة الأولى لفصل الـ Method
    size_t method_end = request_data.find(' ');
    req.method = request_data.substr(0, method_end);

    // نبحث عن المسافة الثانية لفصل الـ URI
    size_t uri_start = method_end + 1;
    size_t uri_end = request_data.find(' ', uri_start);
    req.uri = request_data.substr(uri_start, uri_end - uri_start);

    // نبحث عن نهاية السطر (CRLF) لفصل الـ Version
    size_t version_start = uri_end + 1;
    size_t version_end = request_data.find("\r\n", version_start);
    req.version = request_data.substr(version_start, version_end - version_start);

    return req;
}

int main() {
    // حالة طبيعية (تعمل بنجاح)
    std::string valid_req = "GET/index.htmlHTTP/1.1\r\nHost:localhost\r\n\r\n";
    RequestLine parsed = parse_request_line(valid_req);
    
    std::cout << "Method: " << parsed.method << "\n";
    std::cout << "URI: " << parsed.uri << "\n";
    std::cout << "Version: " << parsed.version << "\n";
    
    return 0;
}