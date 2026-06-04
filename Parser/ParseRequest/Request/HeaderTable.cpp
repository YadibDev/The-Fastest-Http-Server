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


const s_header_slot* HeaderTable::getKnownHeader(HttpTables::eKnownHeader h) const
{
	if (h >= HttpTables::H_UNKNOWN)
		return NULL;
	return &_request.known_headers[h];
}

const s_header_slot* HeaderTable::getUnknownHeader(uint8_t index) const{
	if (index >= SIZE_UNKNOW_HEADER || index == INVALID_INDEX)
		return NULL;
	return &_request.unknown_headers[index];
}

void HeaderTable::linkThisHeader()
{
    for (uint8_t i = 0; i < SIZE_UNKNOW_HEADER && _request.unknown_headers[i].key.len; i++)
    {
        if (_request.unknown_headers[i].Hash == -1)
            continue;

        for (uint8_t j = 0; j < SIZE_UNKNOW_HEADER && _request.unknown_headers[j].key.len; j++)
        {
            if (j == i)
                continue;
            if (_request.unknown_headers[j].Hash == -1)
                continue;

            if (_request.unknown_headers[i].Hash != _request.unknown_headers[j].Hash)
                continue;
            if (!_keysMatch(_request.unknown_headers[i].key, _request.unknown_headers[j].key))
                continue;

            uint8_t last = i;
            while (_request.unknown_headers[last].next != INVALID_INDEX)
                last = _request.unknown_headers[last].next;

            _request.unknown_headers[last].next = j;
            _request.unknown_headers[j].Hash = -1;
        }
    }
}

bool HeaderTable::isDuplicate(uint8_t index) {
	for (uint8_t i = 0; i < SIZE_UNKNOW_HEADER; ++i) {
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
