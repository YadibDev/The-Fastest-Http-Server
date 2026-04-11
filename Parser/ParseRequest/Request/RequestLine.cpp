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

void    RequestLine::reset(uint16_t startOffset)
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

void    RequestLine::selectMethod(const char *buffer, uint16_t size)
{
	if (_offset > size)
		return;

	if (buffer[_offset] == 'G')
		_methodType = HttpTables::M_GET;
	else if (buffer[_offset] == 'P')
		_methodType = HttpTables::M_POST;
	else if (buffer[_offset] == 'D')
		_methodType = HttpTables::M_DELETE;
	else
	{
		_state = STATE_ERROR;
		return;
	}

	_method.Data = (char *)&buffer[_offset];
	_methodIndex = 0;
	_state = STATE_METHOD;
}

void    RequestLine::parseMethod(const char *buffer, uint16_t size)
{
	const char *expected = methodName();

	if (!expected)
	{
		_state = STATE_ERROR;
		return;
	}

	while (expected[_methodIndex] && _offset <= size)
	{
		if (buffer[_offset] != expected[_methodIndex])
		{
			_state = STATE_ERROR;
			return;
		}
		_offset++;
		_methodIndex++;
	}

	if (expected[_methodIndex] != '\0')
		return;
	if (_offset > size)
		return;
	if (!isSpace(buffer[_offset]))
	{
		_state = STATE_ERROR;
		return;
	}

	_method.len = (uint16_t)((char *)&buffer[_offset] - _method.Data);
	_state = STATE_URI;
}

void    RequestLine::parseUri(const char *buffer, uint16_t size)
{
	if (!_uriReady)
	{
		skipSpaceLastIndex(buffer, size, _offset);
		if (_offset > size)
			return;
		_uriParser.init(_offset);
		_uriReady = true;
	}

	_uriParser.parse(buffer, size);

	if (_uriParser.isError())
	{
		_state = STATE_ERROR;
		return;
	}
	if (!_uriParser.isComplete())
		return;

	_offset = _uriParser.getOffset();
	_state = STATE_VERSION;
}

void    RequestLine::parseVersion(const char *buffer, uint16_t size)
{
	const char *expectedPrefix = "HTTP/1.";

	if (!_versionReady)
	{
		skipSpaceLastIndex(buffer, size, _offset);
		if (_offset > size)
			return;
		
		_version.Data = (char *)&buffer[_offset];
		_version.len = 0;
		_versionReady = true;
	}

	while (_versionIndex < 7 && _offset <= size)
	{
		if (buffer[_offset] != expectedPrefix[_versionIndex])
		{
			_state = STATE_ERROR;
			return;
		}
		_offset++;
		_versionIndex++;
	}

	if (_versionIndex < 7)
		return;

	if (!_versionMinorRead)
	{
		if (_offset > size)
			return;
		if (buffer[_offset] != '0' && buffer[_offset] != '1')
		{
			_state = STATE_ERROR;
			return;
		}
		_offset++;
		_versionMinorRead = true;
		
		_version.len = 8;
	}

	_state = STATE_CR;
}

void    RequestLine::parseCR(const char *buffer, uint16_t size)
{
	if (_offset > size)
		return;
	if (buffer[_offset] != '\r')
	{
		_state = STATE_ERROR;
		return;
	}
	_offset++;
	_state = STATE_LF;
}

void    RequestLine::parseLF(const char *buffer, uint16_t size)
{
	if (_offset > size)
		return;
	if (buffer[_offset] != '\n')
	{
		_state = STATE_ERROR;
		return;
	}
	_offset++;
	_state = STATE_COMPLETE;
}

void    RequestLine::parse(const char *buffer, uint16_t size)
{
	if (_state == STATE_COMPLETE || _state == STATE_ERROR)
		return;

	while (_offset <= size)
	{
		uint16_t oldOffset = _offset;
		State oldState = _state;

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

		if (_state == STATE_COMPLETE || _state == STATE_ERROR)
			break;
		if (_offset == oldOffset && _state == oldState)
			break;
	}
}

bool				RequestLine::isComplete() const { return (_state == STATE_COMPLETE); }
bool				RequestLine::isError() const { return (_state == STATE_ERROR); }
HttpTables::eMethod	RequestLine::getMethod() const { return _methodType; }
s_view				RequestLine::getVersion() const { return _version; }
UriParser			RequestLine::getRequestURI() const { return _uriParser; }
uint16_t			RequestLine::getOffset() const { return _offset; }