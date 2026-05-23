#include "Header.hpp"
#include <cctype>


Header::Header(stPollRequest &request) : _request(request)
{
	_state = HttpTables::STATE_KEY;
	_offset = 0;
	_keyStart = 0;
	_valueStart = 0;
	_hash = SEED;
	_emptyLinePending = false;
	_skipSpaceAfterColon = false;
	_currentHeader = HttpTables::H_UNKNOWN;
	_currentUnknownIndex = INVALID_INDEX;
	_indexUnknownHeaders = 0;
}

void    Header::init(uint16_t offset)
{
	_offset = offset;
	_keyStart = offset;
	_valueStart = offset;
	_hash = SEED;
	_emptyLinePending = false;
	_skipSpaceAfterColon = false;
	_currentHeader = HttpTables::H_UNKNOWN;
	_currentUnknownIndex = INVALID_INDEX;
	_indexUnknownHeaders = 0;
	_state = HttpTables::STATE_KEY;
	_error.setStatus(0, "");
}

void	Header::hashStep(char c)
{
	_hash ^= static_cast<unsigned char>(c);
	_hash *= 0x5bd1e995;
	_hash ^= _hash >> 15;
}

HttpTables::eKnownHeader Header::fromHash(uint32_t h)
{
	switch (h)
	{
		case 0x1e5aeec4: return HttpTables::H_HOST;
		case 0x240bbc50: return HttpTables::H_CONTENT_LENGTH;
		case 0x41141d0c: return HttpTables::H_TRANSFER_ENCODING;
		case 0xc947e5d0: return HttpTables::H_CONTENT_TYPE;
		case 0x8fd8a976: return HttpTables::H_CONNECTION;
		case 0x4def559a: return HttpTables::H_COOKIE;
		default: return HttpTables::H_UNKNOWN;
	}
}

bool    Header::isHeaderKeyChar(char c)
{
	unsigned char uc = static_cast<unsigned char>(c);
	if (std::isalnum(uc)) return true;
	switch (c)
	{
		case '!': case '#': case '$': case '%': case '&': case '\'':
		case '*': case '+': case '-': case '.': case '^': case '_':
		case '`': case '|': case '~': return true;
		default: return false;
	}
}

bool    Header::isHeaderValueChar(char c)
{
	unsigned char uc = static_cast<unsigned char>(c);
	return (uc >= 32 && uc <= 126);
}

bool    Header::canRead(uint16_t size) const { return (_offset <= size); }


bool	Header::CheckHostAbsUri(s_view &VHost)
{
	if (!VHost.len)
		return (_error.setStatus(400, "Header Fields Host Is Empty"), false);
	return true;
}

bool    Header::makeUnknownHeader()
{
	_currentUnknownIndex = _indexUnknownHeaders;
	_request.unknown_headers[_currentUnknownIndex].Hash = _hash;
	
	_request.unknown_headers[_currentUnknownIndex].key.Data = (char *)&_request.request_metadata[_keyStart];
	_request.unknown_headers[_currentUnknownIndex].key.len = (_offset - 1) - _keyStart;
	
	_request.unknown_headers[_currentUnknownIndex].next = INVALID_INDEX;
	return true;
}

bool    Header::makeKnownHeader()
{
	
	if ((_currentHeader == HttpTables::H_HOST && _request.known_headers[HttpTables::H_HOST].Hash != -1)
			|| (_currentHeader == HttpTables::H_CONTENT_LENGTH && _request.known_headers[HttpTables::H_CONTENT_LENGTH].Hash != -1))
				return (_error.setStatus(400, "The Header Does Not Accept More Than One Value"), false);

	if (_request.known_headers[_currentHeader].key.Data == NULL)
	{
		_request.known_headers[_currentHeader].Hash = _hash;
		_request.known_headers[_currentHeader].key.Data = (char *)&_request.request_metadata[_keyStart];
		_request.known_headers[_currentHeader].key.len = (_offset - 1) - _keyStart;
	}
	else
	{
		if (!makeUnknownHeader())
			return false;
		uint8_t next = _request.known_headers[_currentHeader].next;
		if (next == INVALID_INDEX)
			_request.known_headers[_currentHeader].next = _currentUnknownIndex;
		else
		{
			while (_request.unknown_headers[next].next != INVALID_INDEX)
				next = _request.unknown_headers[next].next;
			_request.unknown_headers[next].next = _currentUnknownIndex;
		}
	}
	return true;
}

bool    Header::selectHeaderSlot()
{
	_currentHeader = fromHash(_hash);
	_currentUnknownIndex = INVALID_INDEX;
	if (_currentHeader != HttpTables::H_UNKNOWN)
	{
		if (!makeKnownHeader())
			return false;
	}
	else
	{
		if (!makeUnknownHeader())
			return false;
	}

	_hash = SEED;
	_skipSpaceAfterColon = true;
	return true;
}

bool    Header::storeValue()
{
	uint16_t tmpOffset = _offset;

	while (HelperFunctions::isspaceTabOrSp(_request.request_metadata[tmpOffset - 1]))
		--tmpOffset;

	uint16_t valueLen = tmpOffset - _valueStart;
	if (_currentHeader != HttpTables::H_UNKNOWN && _currentUnknownIndex == INVALID_INDEX)
	{
		_request.known_headers[_currentHeader].val.Data = (char *)&_request.request_metadata[_valueStart];
		_request.known_headers[_currentHeader].val.len = valueLen;
		if (_currentHeader == HttpTables::H_HOST)
			if (!CheckHostAbsUri(_request.known_headers[HttpTables::H_HOST].val))
				return false;
	}
	else
	{
		_request.unknown_headers[_currentUnknownIndex].val.Data = (char *)&_request.request_metadata[_valueStart];
		_request.unknown_headers[_currentUnknownIndex].val.len = valueLen;
		_indexUnknownHeaders++;
	}
	return true;
}

bool    Header::parseKey(uint16_t size)
{
	if (_indexUnknownHeaders >= SIZE_UNKNOW_HEADER)
		return (_error.setStatus(431, "Request Header Fields Too Large"), false);
	while (canRead(size) && _state == HttpTables::STATE_KEY)
	{
		char c = _request.request_metadata[_offset];
		if (c == ':')
		{
			_offset++;
			if (!selectHeaderSlot())
				return false;
			_state = HttpTables::STATE_VALUE;
			return true;
		}
		if (!isHeaderKeyChar(c))
			return (_error.setStatus(400, "Bad Request"), false);
		hashStep(tolower(c));
		_offset++;
	}
	return true;
}

bool    Header::parseValue(uint16_t size)
{
	if (_skipSpaceAfterColon)
	{
		skipSpaceLastIndex(_request.request_metadata, size, _offset);
		if (!canRead(size))
			return true;
		_valueStart = _offset;
		_skipSpaceAfterColon = false;
	}
	while (canRead(size) && _state == HttpTables::STATE_VALUE)
	{
		char c = _request.request_metadata[_offset];
		if (c == '\r')
		{
			_state = HttpTables::STATE_CR;
			return storeValue();
		}
		if (!isHeaderValueChar(c))
			return (_error.setStatus(400, "Bad Request"), false);
		_offset++;
	}
	return true;
}

bool    Header::parseCR(uint16_t size)
{
	if (!canRead(size)) return true;
	if (_request.request_metadata[_offset] != '\r')
		return (_error.setStatus(400, "Bad Request"), false);
	_request.request_metadata[_offset] = '\0';
	_offset++;
	_state = HttpTables::STATE_LF;
	return true;
}

bool    Header::parseLF(uint16_t size)
{
	if (!canRead(size)) return true;
	if (_request.request_metadata[_offset] != '\n')
		return (_error.setStatus(400, "Bad Request"), false);
	_offset++;
	_state = HttpTables::STATE_DECISION;
	return true;
}

bool    Header::parseDecision(uint16_t size)
{
	if (!canRead(size)) return true;
	if (_request.request_metadata[_offset] == '\r')
	{
		_emptyLinePending = true;
		_state = HttpTables::STATE_CR;
		return true;
	}
	_emptyLinePending = false;
	_keyStart = _offset;
	_hash = SEED;
	_currentHeader = HttpTables::H_UNKNOWN;
	_currentUnknownIndex = INVALID_INDEX;
	_state = HttpTables::STATE_KEY;
	return true;
}

void    Header::Parse(uint16_t size)
{
	while (canRead(size))
	{
		if (_state == HttpTables::STATE_KEY) parseKey(size);
		else if (_state == HttpTables::STATE_VALUE) parseValue(size);
		else if (_state == HttpTables::STATE_CR) parseCR(size); // \r
		else if (_state == HttpTables::STATE_LF)
		{
			parseLF(size); // \n
			if (_state == HttpTables::STATE_DECISION && _emptyLinePending)
				_state = HttpTables::STATE_COMPLETE;
		}
		else if (_state == HttpTables::STATE_DECISION) parseDecision(size);
		else break;
		if (_error.isError() || _state == HttpTables::STATE_COMPLETE)
			break;
	}
}

uint16_t    Header::getOffset() const { return _offset; }
bool        Header::isError() const { return _error.isError(); }
bool        Header::isComplete() const { return (_state == HttpTables::STATE_COMPLETE); }
HttpError	Header::getError() const { return _error; }