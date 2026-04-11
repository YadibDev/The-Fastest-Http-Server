#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include "../URI/NUriParser.hpp"

class RequestLine {
private:
    enum State { STATE_START, STATE_METHOD, STATE_URI, STATE_VERSION, STATE_CR, STATE_LF, STATE_COMPLETE, STATE_ERROR };

    State _state;
    HttpTables::eMethod _methodType;
    uint16_t _offset;
    bool _uriReady;
    bool _versionReady;
    bool _versionMinorRead;
    s_view _method;
    s_view _version;
    uint8_t _methodIndex;
    uint8_t _versionIndex;
    UriParser _uriParser;

    const char *methodName() const;
    void selectMethod(const char *buffer, uint16_t size);
    void parseMethod(const char *buffer, uint16_t size);
    void parseUri(const char *buffer, uint16_t size);
    void parseVersion(const char *buffer, uint16_t size);
    void parseCR(const char *buffer, uint16_t size);
    void parseLF(const char *buffer, uint16_t size);

public:
    RequestLine();
    void reset(uint16_t startOffset = 0);
    void parse(const char *buffer, uint16_t size);
    bool isComplete() const;
    bool isError() const;
    HttpTables::eMethod getMethod() const;
    s_view getVersion() const;
    UriParser getRequestURI() const;
    uint16_t getOffset() const;
};

#endif