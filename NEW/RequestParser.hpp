#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "RequestLine.hpp"
#include "Header.hpp"

class RequestParser {
private:
    enum State { STATE_REQUEST_LINE, STATE_HEADERS, STATE_BODY, STATE_COMPLETE, STATE_ERROR };
    stPollRequest &_request;
    State _state;
    uint16_t _offset;
    RequestLine _requestLine;
    Header _header;

    void ParseRequestLine(uint16_t size);
    void ParseHeader(uint16_t size);
    void ParseBody(uint16_t);

public:
    RequestParser(stPollRequest &request);
    void init(uint16_t offset = 0);
    void Parse(uint16_t size);
    bool isComplete() const;
    bool isError() const;
    RequestLine getRequestLine() const;
};

#endif