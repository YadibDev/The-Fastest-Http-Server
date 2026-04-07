#include "RequestParser.hpp"
#include "Utils.hpp"

int main(void) {
    PollOfClient client;
    const char *raw =
    "GET /api/users/123 HTTP/1.1\r\n"
    "Host: api.example.com\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\r\n"
    "Content-Length: 20\r\n"
    "Accept: application/json\r\n"
    "Accept-Language: en-US,en;q=0.9\r\n"
    "\r\n";

    memcpy(client.request_metadata, raw, strlen(raw) + 1);
    stPollRequest req = makeRequest(client);
    RequestParser parser(req);
    parser.init(0);

    for (int i = 0; client.request_metadata[i]; i++) {
        parser.Parse(i);
        if (parser.isComplete() || parser.isError()) break;
    }

    if (parser.isError()) { std::cout << "parse error\n"; return 1; }

    if (parser.isComplete()) {
        std::cout << "Method: "; print_view(client.request_metadata, parser.getRequestLine().getMethod()); std::cout << "\n";
        std::cout << "Path: "; print_view(client.request_metadata, parser.getRequestLine().getRequestURI().getPath()); std::cout << "\n";
        std::cout << "Version: "; print_view(client.request_metadata, parser.getRequestLine().getVersion()); std::cout << "\n";
        std::cout << "Host key: "; print_view(client.request_metadata, req.known_headers[HttpTables::H_HOST].key); std::cout << "\n";
        std::cout << "Host value: "; print_view(client.request_metadata, req.known_headers[HttpTables::H_HOST].val); std::cout << "\n";
    }
    return 0;
}