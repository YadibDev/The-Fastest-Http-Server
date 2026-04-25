#include "NUriParser.hpp"

#define DEFAULT_PORT 80

UriParser::UriParser() {}

void UriParser::init(uint16_t startOffset) {
	_state          = STATE_SCHEMA;
	_pctReturnState = STATE_SCHEMA;
	_offset         = startOffset;
	_complete       = false;
	_pctDigitsRead  = 0;
	_shemaIndex     = 0;
	_port           = DEFAULT_PORT;

	_authority.Data = NULL; _authority.len = 0;
	_host.Data      = NULL; _host.len      = 0;
	_path.Data      = NULL; _path.len      = 0;
	_query.Data     = NULL; _query.len     = 0;
	_fragment.Data  = NULL; _fragment.len  = 0;

	_error = HttpError();
}


bool	UriParser::isAbsoluteURI(const std::string& uri)
{
    if (uri.empty())
        return false;

    size_t colonPos = uri.find(':');
    
    if (colonPos == std::string::npos || colonPos == 0)
        return false;

    if (!std::isalpha(uri[0]))
        return false;

    for (size_t i = 1; i < colonPos; ++i) {
        char c = uri[i];
        if (!std::isalnum(c) && c != '+' && c != '-' && c != '.')
            return false;
    }

    if (uri.find('#') != std::string::npos)
        return false;

    if (colonPos + 1 == uri.length())
        return false;

    return true;
}



bool UriParser::isUnreserved(char c) {
	unsigned char uc = static_cast<unsigned char>(c);
	return std::isalnum(uc) || c == '-' || c == '.' || c == '_' || c == '~';
}

bool UriParser::isSubDelim(char c) {
	return c == '!' || c == '$' || c == '&' || c == '\'' ||
		   c == '(' || c == ')' || c == '*' || c == '+' ||
		   c == ',' || c == ';' || c == '=';
}

bool UriParser::isPchar(char c) {
	return isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@';
}

bool UriParser::isHex(char c) {
	return (c >= '0' && c <= '9') ||
		   (c >= 'a' && c <= 'f') ||
		   (c >= 'A' && c <= 'F');
}

bool UriParser::isPathChar(char c)     { return isPchar(c) || c == '/'; }
bool UriParser::isQueryChar(char c) { return isPchar(c) || c == '/' || c == '?' || c == '=' || c == '&'; }
bool UriParser::isFragmentChar(char c) { return isPchar(c) || c == '/' || c == '?'; }

bool UriParser::validateIPv4(const char* data, size_t len)
{
	int octets = 0, val = 0, digits = 0;
	for (size_t i = 0; i < len; ++i) {
		char c = data[i];
		if (c == '.')
		{
			if (digits == 0)
			return (_error.setStatus(400, "Empty IPv4 octet"), false);
			++octets;
			val = 0;
			digits = 0;
		}
		else if (std::isdigit(static_cast<unsigned char>(c)))
		{
			if (digits == 1 && val == 0)
				return (_error.setStatus(400, "Leading zero in IPv4"), false);
			val = val * 10 + (c - '0');
			if (val > 255)
				return (_error.setStatus(400, "IPv4 octet > 255"), false);
			++digits;
			if (digits > 3)
				return (_error.setStatus(400, "Too many digits in IPv4 octet"), false);
		}
		else
			return (_error.setStatus(400, "Invalid character in IPv4"), false);
	}
	if (octets != 3 || digits == 0) { _error.setStatus(400, "Invalid IPv4 format"); return false; }
	return true;
}

bool UriParser::validateRegName(const char* data, size_t len)
{
	if (len == 0)
		return (_error.setStatus(400, "Empty host name"), false);
	for (size_t i = 0; i < len; ++i) {
		unsigned char c = data[i];
		if (c == '%') {
			if (i + 2 >= len || !isHex(data[i+1]) || !isHex(data[i+2]))
				return (_error.setStatus(400, "Invalid percent-encoding in host"), false);

			i += 2;
			continue;
		}
		if (!std::isalnum(c) && c != '-' && c != '.')
			return (_error.setStatus(400, "Invalid character in host name"), false);
	}
	return true;
}

void UriParser::finishAuthority(const char* buffer) {
	if (_authority.Data && _authority.len == 0)
		_authority.len = static_cast<uint16_t>(&buffer[_offset] - _authority.Data);
}

void UriParser::finishPath(const char* buffer) {
	if (_path.Data && _path.len == 0)
		_path.len = static_cast<uint16_t>(&buffer[_offset] - _path.Data);
}

void UriParser::finishQuery(const char* buffer) {
	if (_query.Data && _query.len == 0)
		_query.len = static_cast<uint16_t>(&buffer[_offset] - _query.Data);
}

void UriParser::finishFragment(const char* buffer) {
	if (_fragment.Data && _fragment.len == 0)
		_fragment.len = static_cast<uint16_t>(&buffer[_offset] - _fragment.Data);
}

void UriParser::enterPctEncoded(State returnState) {
	_pctReturnState = returnState;
	_pctDigitsRead  = 0;
	++_offset;
	_state = STATE_PCT_ENCODED;
}

void UriParser::parseScheme(const char* buffer, uint16_t size)
{
	if (_offset <= size && buffer[_offset] == '/')
	{
		_state = STATE_PATH;
		return;
	}

	static const char http[] = "http://";
	while (_shemaIndex < 7 && _offset <= size) {
		if (buffer[_offset] != http[_shemaIndex]) {
			_error.setStatus(400, "Invalid scheme");
			_state = STATE_ERROR;
			return;
		}
		++_shemaIndex;
		++_offset;
	}
	if (_shemaIndex == 7)
	{
		_AuthorityExist = true;
		_state = STATE_AUTHORITY;
		_shemaIndex = 0;
	}
}

void UriParser::parseAuthority(const char* buffer, uint16_t size) {
	if (!_authority.Data)
		_authority.Data = const_cast<char*>(&buffer[_offset]);

	const char* atPos = NULL;
	for (uint16_t i = _offset; i < size; ++i) {
		if (buffer[i] == '@') { atPos = &buffer[i]; break; }
	}
	if (atPos != NULL)
		_offset = static_cast<uint16_t>(atPos - buffer + 1);

	_state = STATE_HOST;
}

void UriParser::_parseHost(const char* buffer, uint16_t size) {
	size_t start = _offset;
	while (_offset <= size) {
		char c = buffer[_offset];
		if (c == ':' || c == '/' || c == '?' || c == '#') break;
		++_offset;
	}
	size_t len = _offset - start;
	if (len == 0) { _error.setStatus(400, "Empty host"); _state = STATE_ERROR; return; }

	bool valid = false;
	if (validateIPv4(&buffer[start], len)) valid = true;
	else {
		_error = HttpError();
		valid = validateRegName(&buffer[start], len);
	}
	if (!valid) { _state = STATE_ERROR; return; }

	_host.Data = const_cast<char*>(&buffer[start]);
	_host.len  = static_cast<uint16_t>(len);

	if (_offset <= size && buffer[_offset] == ':') {
		_state = STATE_PORT;
		++_offset;
	} else {
		finishAuthority(buffer);
		_state = STATE_PATH;
	}
}

void UriParser::_parsePort(const char* buffer, uint16_t size) {
	unsigned int portNum = 0;
	bool hasDigit = false;
	while (_offset <= size && _state == STATE_PORT) {
		char c = buffer[_offset];
		if (c == '/' || c == '?' || c == '#') break;
		if (!std::isdigit(static_cast<unsigned char>(c))) {
			_error.setStatus(400, "Invalid port character");
			_state = STATE_ERROR;
			return;
		}
		portNum = portNum * 10 + (c - '0');
		if (portNum > 65535) {
			_error.setStatus(400, "Port out of range");
			_state = STATE_ERROR;
			return;
		}
		hasDigit = true;
		++_offset;
	}
	if (!hasDigit) {
		_error.setStatus(400, "Empty port");
		_state = STATE_ERROR;
		return;
	}
	_port = static_cast<unsigned short>(portNum);
	finishAuthority(buffer);
	_state = STATE_PATH;
}

void UriParser::parsePath(const char* buffer, uint16_t size) {
	if (!_path.Data) _path.Data = const_cast<char*>(&buffer[_offset]);

	while (_offset <= size && _state == STATE_PATH) {
		char c = buffer[_offset];

		if (c == ' ' || c == '\t') {
			finishPath(buffer);
			_complete = true;
			return;
		}
		if (c == '?') {
			finishPath(buffer);
			++_offset;
			_state = STATE_QUERY;
			return;
		}
		if (c == '#') {
			finishPath(buffer);
			++_offset;
			_state = STATE_FRAGMENT;
			return;
		}
		if (c == '%') {
			enterPctEncoded(STATE_PATH);
			return;
		}
		if (!isPathChar(c)) {
			_error.setStatus(400, "Invalid path character");
			_state = STATE_ERROR;
			return;
		}
		++_offset;
	}
}

void UriParser::parseQuery(const char* buffer, uint16_t size) {
	if (!_query.Data) _query.Data = const_cast<char*>(&buffer[_offset]);

	while (_offset <= size && _state == STATE_QUERY) {
		char c = buffer[_offset];

		if (c == ' ' || c == '\t') {
			finishQuery(buffer);
			_complete = true;
			return;
		}
		if (c == '#') {
			finishQuery(buffer);
			++_offset;
			_state = STATE_FRAGMENT;
			return;
		}
		if (c == '%') {
			enterPctEncoded(STATE_QUERY);
			return;
		}
		if (!isQueryChar(c)) {
			_error.setStatus(400, "Invalid query character");
			_state = STATE_ERROR;
			return;
		}
		++_offset;
	}
}

void UriParser::parseFragment(const char* buffer, uint16_t size) {
	if (!_fragment.Data) _fragment.Data = const_cast<char*>(&buffer[_offset]);

	while (_offset <= size && _state == STATE_FRAGMENT) {
		char c = buffer[_offset];

		if (c == ' ' || c == '\t') {
			finishFragment(buffer);
			_complete = true;
			return;
		}
		if (c == '%') {
			enterPctEncoded(STATE_FRAGMENT);
			return;
		}
		if (!isFragmentChar(c)) {
			_error.setStatus(400, "Invalid fragment character");
			_state = STATE_ERROR;
			return;
		}
		++_offset;
	}
}

void UriParser::parsePctEncoded(const char* buffer, uint16_t size) {
	while (_offset <= size && _pctDigitsRead < 2) {
		if (!isHex(buffer[_offset])) {
			_error.setStatus(400, "Invalid hex digit");
			_state = STATE_ERROR;
			return;
		}
		++_pctDigitsRead;
		++_offset;
	}
	if (_pctDigitsRead == 2) {
		_state = _pctReturnState;
		_pctDigitsRead = 0;
	}
}

void UriParser::parse(const char* buffer, uint16_t size)
{

	while (_offset <= size && !_complete && _state != STATE_ERROR)
	{

		switch (_state)
		{
			case STATE_SCHEMA:    parseScheme(buffer, size);   break;
			case STATE_AUTHORITY: parseAuthority(buffer, size); break;
			case STATE_HOST:      _parseHost(buffer, size);    break;
			case STATE_PORT:      _parsePort(buffer, size);    break;
			case STATE_PATH:      parsePath(buffer, size);     break;
			case STATE_QUERY:     parseQuery(buffer, size);    break;
			case STATE_FRAGMENT:  parseFragment(buffer, size); break;
			case STATE_PCT_ENCODED: parsePctEncoded(buffer, size); break;
			default: break;
		}

	}

	if (_offset == size && !_complete && _state != STATE_ERROR)
	{
		if (_state == STATE_HOST || _state == STATE_PORT)
		{
			finishAuthority(buffer);
			_state = STATE_PATH;
		}
		if (_state == STATE_PATH) finishPath(buffer);
		else if (_state == STATE_QUERY) finishQuery(buffer);
		else if (_state == STATE_FRAGMENT) finishFragment(buffer);
		_complete = true;
	}
}

bool                    UriParser::isComplete()		const { return _complete; }
bool					UriParser::isError()		const { return _state == STATE_ERROR; }
uint16_t				UriParser::getOffset()		const { return _offset; }
unsigned short			UriParser::getPort()		const { return _port; }
const s_view&			UriParser::getAuthority()	const { return _authority; }
const s_view&			UriParser::getHost()		const { return _host; }
const s_view&			UriParser::getPath()		const { return _path; }
const s_view&			UriParser::getQuery()		const { return _query; }
const s_view&			UriParser::getFragment()	const { return _fragment; }
const HttpError&		UriParser::getError()		const { return _error; }
	bool				UriParser::AuthorityExist()			{ return _AuthorityExist; }
