#include "HeaderTable.hpp"

HeaderTable::HeaderTable(stPollRequest& req) : _request(req) {}

HeaderTable& HeaderTable::operator=(const HeaderTable& rhs)
{
    if (this != &rhs)
    { 
        this->_request = rhs._request;
    }
    return *this;
}

HeaderTable::~HeaderTable() {}

bool HeaderTable::_keysMatch(const s_view& k1, const s_view& k2) const {
    if (k1.len != k2.len || k1.Data == NULL || k2.Data == NULL)
        return false;
    return (std::memcmp(k1.Data, k2.Data, k1.len) == 0);
}

const s_header_slot* HeaderTable::getKnownHeader(HttpTables::eKnownHeader h) {
    if (h >= HttpTables::H_UNKNOWN)
        return NULL;
    return &_request.known_headers[h];
}

const s_header_slot* HeaderTable::getUnknownHeader(uint8_t index) {
    if (index >= _request.sizeUnknownHeaders || index == INVALID_INDEX)
        return NULL;
    return &_request.unknown_headers[index];
}

void HeaderTable::linkThisHeader(uint8_t newIndex, uint8_t currentIndex)
{
    if (newIndex >= _request.sizeUnknownHeaders)
        return;

    s_header_slot& newHdr = _request.unknown_headers[newIndex];

    for (uint8_t i = 0; i < currentIndex; ++i)
    {
        if (i == newIndex)
            continue;

        s_header_slot& currentHdr = _request.unknown_headers[i];

        if (newHdr.Hash == currentHdr.Hash) {
            
            if (_keysMatch(newHdr.key, currentHdr.key))
            {
                
                uint8_t last = i;
                while (_request.unknown_headers[last].next != INVALID_INDEX)
                    last = _request.unknown_headers[last].next;
                
                if (last != newIndex)
                    _request.unknown_headers[last].next = newIndex;
                return;
            }
        }
    }
}

bool HeaderTable::isDuplicate(uint8_t index) {
    for (uint8_t i = 0; i < _request.sizeUnknownHeaders; ++i) {
        if (i != index && _request.unknown_headers[i].next == index)
            return true;
    }
    return false;
}

uint8_t HeaderTable::countOccurrences(HttpTables::eKnownHeader h) {
    const s_header_slot* current = getKnownHeader(h);
    if (!current || current->key.Data == NULL)
        return 0;
    
    uint8_t count = 1;
    uint8_t nextIdx = current->next;
    while (nextIdx != INVALID_INDEX) {
        count++;
        nextIdx = _request.unknown_headers[nextIdx].next;
    }
    return count;
}