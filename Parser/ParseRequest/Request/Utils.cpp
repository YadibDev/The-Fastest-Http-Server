#include "Utils.hpp"

bool isSpace(char c) { return (c == ' ' || c == '\t'); }

void skipSpaceLastIndex(const char *buffer, uint16_t size, uint16_t &offset) {
    while (offset <= size && isSpace(buffer[offset])) offset++;
}

int compare_view(const s_view& view, const std::string& other) {
    if (!view.Data) return -1;
    
    size_t view_len = view.len;
    size_t other_len = other.length();
    size_t min_len = (view_len < other_len) ? view_len : other_len;
    
    int res = std::memcmp(view.Data, other.data(), min_len);
    if (res == 0) {
        if (view_len < other_len) return -1;
        if (view_len > other_len) return 1;
    }
    return res;
}

stPollRequest makeRequest(PollOfClient &client) {
    stPollRequest req;
    req.request_metadata = client.request_metadata;
    req.known_headers = client.known_headers;
    req.unknown_headers = client.unknown_headers;
    req.sizeUnknownHeaders = 25;
    req.io_chunk = client.io_chunk;

    for (int i = 0; i < HttpTables::H_COUNT; i++) {
        req.known_headers[i] = s_header_slot();
        req.known_headers[i].key.Data = NULL;
        req.known_headers[i].key.len = 0;
        req.known_headers[i].next = INVALID_INDEX;
    }
    for (int i = 0; i < (int)req.sizeUnknownHeaders; i++) {
        req.unknown_headers[i] = s_header_slot();
        req.unknown_headers[i].key.Data = NULL;
        req.unknown_headers[i].key.len = 0;
        req.unknown_headers[i].next = INVALID_INDEX;
    }
    return req;
}

void print_view(const s_view &view) {
    if (!view.Data || view.len == 0) { 
        std::cout << "(empty)"; 
        return; 
    }
    for (uint16_t i = 0; i < view.len; i++) {
        std::cout << view.Data[i];
    }
}