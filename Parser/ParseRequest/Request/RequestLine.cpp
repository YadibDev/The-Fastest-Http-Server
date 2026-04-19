#include "RequestLine.hpp"

RequestLine::RequestLine()
	: _state(STATE_START),
	  _methodType(HttpTables::M_UNKNOWN),
	  _offset(0),
	  _uriReady(false),
	  _versionReady(false),
	  _versionMinorRead(false),
	  _methodIndex(0),
	  _versionIndex(0)
{
	_method.Data = NULL;
	_method.len = 0;
	_version.Data = NULL;
	_version.len = 0;
}

void    RequestLine::init(uint16_t startOffset)
{
	_state = STATE_START;
	_methodType = HttpTables::M_UNKNOWN;
	_offset = startOffset;
	_uriReady = false;
	_versionReady = false;
	_versionMinorRead = false;
	_methodIndex = 0;
	_versionIndex = 0;
	_method.Data = NULL;
	_method.len = 0;
	_version.Data = NULL;
	_version.len = 0;
	
	_error.setStatus(0, "");
	_uriParser.init(startOffset);
}

const char* RequestLine::methodName() const
{
	if (_methodType == HttpTables::M_GET)
		return "GET";
	if (_methodType == HttpTables::M_POST)
		return "POST";
	if (_methodType == HttpTables::M_DELETE)
		return "DELETE";
	return 0;
}

bool    RequestLine::selectMethod(const char *buffer, uint16_t size)
{
	if (_offset > size)
		return true;

	if (buffer[_offset] == 'G')
		_methodType = HttpTables::M_GET;
	else if (buffer[_offset] == 'P')
		_methodType = HttpTables::M_POST;
	else if (buffer[_offset] == 'D')
		_methodType = HttpTables::M_DELETE;
	else
		return false;

	_method.Data = (char *)&buffer[_offset];
	_methodIndex = 0;
	_state = STATE_METHOD;
	return true;
}

bool    RequestLine::parseMethod(const char *buffer, uint16_t size)
{
	const char *expected = methodName();

	if (!expected)
		return (_error.setStatus(405, "Method Not Allowed"), false);

	while (expected[_methodIndex] && _offset <= size)
	{
		if (buffer[_offset] != expected[_methodIndex])
			return (_error.setStatus(405, "Method Not Allowed"), false);
		_offset++;
		_methodIndex++;
	}

	if (expected[_methodIndex] != '\0')
		return true;
	if (_offset > size)
		return true;
	if (!isSpace(buffer[_offset]))
		return (_error.setStatus(405, "Method Not Allowed"), false);

	_method.len = (uint16_t)((char *)&buffer[_offset] - _method.Data);
	_state = STATE_URI;
	return true;
}

bool    RequestLine::parseUri(const char *buffer, uint16_t size)
{
	if (!_uriReady)
	{
		skipSpaceLastIndex(buffer, size, _offset);
		if (_offset > size)
			return true;
		_uriParser.init(_offset);
		_uriReady = true;
	}

	_uriParser.parse(buffer, size);

	if (_uriParser.isError())
	{
		_error = _uriParser.getError();
		return false;
	}
	if (!_uriParser.isComplete())
		return true;

	_offset = _uriParser.getOffset();
	_state = STATE_VERSION;
	return true;
}

bool    RequestLine::parseVersion(const char *buffer, uint16_t size)
{
	const char *expectedPrefix = "HTTP/1.";

	if (!_versionReady)
	{
		skipSpaceLastIndex(buffer, size, _offset);
		if (_offset > size)
			return true;
		
		_version.Data = (char *)&buffer[_offset];
		_version.len = 0;
		_versionReady = true;
	}

	while (_versionIndex < 7 && _offset <= size)
	{
		if (buffer[_offset] != expectedPrefix[_versionIndex])
			return (_error.setStatus(505, "HTTP Version Not Supported") , false);
		_offset++;
		_versionIndex++;
	}

	if (_versionIndex < 7)
		return true;

	if (!_versionMinorRead)
	{
		if (_offset > size)
			return true;
		if (buffer[_offset] != '0' && buffer[_offset] != '1')
			return (_error.setStatus(505, "HTTP Version Not Supported") , false);

		_offset++;
		_versionMinorRead = true;
		
		_version.len = 8;
	}

	_state = STATE_CR;
	return true;
}

bool    RequestLine::parseCR(const char *buffer, uint16_t size)
{
	if (_offset > size)
		return true;
	if (buffer[_offset] != '\r')
		return (_error.setStatus(400, "Bad Request") , false);

	_offset++;
	_state = STATE_LF;
	return true;
}

bool    RequestLine::parseLF(const char *buffer, uint16_t size)
{
	if (_offset > size)
		return true;
	if (buffer[_offset] != '\n')
		return (_error.setStatus(400, "Bad Request") , false);

	_offset++;
	_state = STATE_COMPLETE;
	return true;
}

bool    RequestLine::parse(const char *buffer, uint16_t size)
{
	if (_state == STATE_COMPLETE)
		return true;

	while (_offset <= size)
	{
		if (_state == STATE_START)
			selectMethod(buffer, size);
		else if (_state == STATE_METHOD)
			parseMethod(buffer, size);
		else if (_state == STATE_URI)
			parseUri(buffer, size);
		else if (_state == STATE_VERSION)
			parseVersion(buffer, size);
		else if (_state == STATE_CR)
			parseCR(buffer, size);
		else if (_state == STATE_LF)
			parseLF(buffer, size);
		else
			break;

		if (_state == STATE_COMPLETE)
			break;
		else if (_error.isError())
			return false;
	}
	return true;
}

bool				RequestLine::isComplete() const { return (_state == STATE_COMPLETE); }
bool				RequestLine::isError() const { return _error.isError(); }
HttpTables::eMethod	RequestLine::getMethod() const { return _methodType; }
s_view				RequestLine::getVersion() const { return _version; }
const UriParser		&RequestLine::getRequestURI() const { return _uriParser; }
uint16_t			RequestLine::getOffset() const { return _offset; }
HttpError			RequestLine::getError() const { return _error; }
