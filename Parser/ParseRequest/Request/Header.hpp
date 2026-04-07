#ifndef HEADER_HPP
#define HEADER_HPP

#include "HttpTypes.hpp"
#include "Utils.hpp"

class Header {
private:
    #define SEED 0x12345678
    enum State { STATE_KEY, STATE_VALUE, STATE_CR, STATE_LF, STATE_DECISION, STATE_COMPLETE, STATE_ERROR };

    stPollRequest &_request;
    uint8_t _state;
    uint16_t _offset;
    uint16_t _keyStart;
    uint16_t _valueStart;
    uint32_t _hash;
    bool emptyLinePending;
    bool skipSpaceAfterColon;
    HttpTables::eKnownHeader _currentHeader;
    uint8_t _currentUnknownIndex;
    uint8_t _indexUnknownHeaders;

    void hashStep(char c);
    HttpTables::eKnownHeader fromHash(uint32_t h);
    bool isHeaderKeyChar(char c);
    bool isHeaderValueChar(char c);
    bool canRead(uint16_t size) const;
    void makeUnknownHeader();
    void makeKnownHeader();
    void selectHeaderSlot();
    void storeValue();
    void parseKey(uint16_t size);
    void parseValue(uint16_t size);
    void parseCR(uint16_t size);
    void parseLF(uint16_t size);
    void parseDecision(uint16_t size);

public:
    Header(stPollRequest &request);
    void init(uint16_t offset);
    void Parse(uint16_t size);
    uint16_t getOffset() const;
    bool isError() const;
    bool isComplete() const;
};

#endif