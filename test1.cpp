#include <cctype>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace std;

/* ============================================================
** Tables
** ============================================================ */

class HttpTables
{
public:
	static INVALID_INDEX = 255;

	enum eMethod
	{
		M_GET,
		M_POST,
		M_DELETE,
		M_METHOD_COUNT,
		M_UNKNOWN
	};

	enum eKnownHeader
	{
		H_HOST,
		H_CONTENT_LENGTH,
		H_TRANSFER_ENCODING,
		H_CONTENT_TYPE,
		H_CONNECTION,
		H_EXPECT,
		H_AUTHORIZATION,
		H_COOKIE,
		H_ACCEPT_ENCODING,
		H_COUNT,
		H_UNKNOWN
	};

	static const char **methods()
	{
		static const char *arr[M_METHOD_COUNT] =
		{
			"GET",
			"POST",
			"DELETE"
		};
		return arr;
	}

	static const char **headers()
	{
		static const char *arr[H_COUNT] =
		{
			"Host",
			"Content-Length",
			"Transfer-Encoding",
			"Content-Type",
			"Connection",
			"Expect",
			"Authorization",
			"Cookie",
			"Accept-Encoding",
		};
		return arr;
	}
};

/* ============================================================
** Helpers
** ============================================================ */

static bool	isSpace(char c)
{
	return (c == ' ' || c == '\t');
}

/* size = last valid index */
static void	skipSpaceLastIndex(const char *buffer, uint16_t size, uint16_t &offset)
{
	while (offset <= size && isSpace(buffer[offset]))
		offset++;
}

/* ============================================================
** Views / Slots
** ============================================================ */

struct s_view
{
	uint16_t	Offsets;
	uint16_t	len;

	s_view() : Offsets(0), len(0) {}
};

struct s_header_slot
{
	s_view		key;
	s_view		val;
	uint8_t		next;
	uint32_t	Hash;

	s_header_slot() : next(INVALID_INDEX), Hash(0) {}
};

struct PollOfClient
{
	char			request_metadata[16384];
	char			io_chunk[8192];
	s_header_slot	known_headers[HttpTables::H_COUNT];
	s_header_slot	unknown_headers[25];
	char			Response_metadata[16384];
	uint16_t		read_offset;
};

struct stPollRequest
{
	char			*request_metadata;
	s_header_slot	*known_headers;
	s_header_slot	*unknown_headers;
	uint8_t			sizeUnknownHeaders;
	char			*io_chunk;
};

int compare_view(const s_view& view, const char* base_buffer, const std::string& other) {
	const char* view_ptr = base_buffer + view.Offsets;
	size_t view_len = view.len;
	size_t other_len = other.length();

	size_t min_len = (view_len < other_len) ? view_len : other_len;
	
	int res = std::memcmp(view_ptr, other.data(), min_len);

	if (res == 0)
	{
		if (view_len < other_len) return -1;
		if (view_len > other_len) return 1;
	}
	return res;
}

static stPollRequest makeRequest(PollOfClient &client)
{
	stPollRequest req;

	req.request_metadata = client.request_metadata;
	req.known_headers = client.known_headers;
	req.unknown_headers = client.unknown_headers;
	req.sizeUnknownHeaders = 25;
	req.io_chunk = client.io_chunk;

	for (int i = 0; i < HttpTables::H_COUNT; i++)
	{
		req.known_headers[i] = s_header_slot();
		req.known_headers[i].key.Offsets = INVALID_INDEX;
		req.known_headers[i].next = INVALID_INDEX;
	}

	for (int i = 0; i < req.sizeUnknownHeaders; i++)
	{
		req.unknown_headers[i] = s_header_slot();
		req.unknown_headers[i].key.Offsets = INVALID_INDEX;
		req.unknown_headers[i].next = INVALID_INDEX;
	}

	return req;
}

static void print_view(const char *buffer, const s_view &view)
{
	if (!buffer || view.len == 0)
	{
		std::cout << "(empty)";
		return;
	}
	for (uint16_t i = 0; i < view.len; i++)
		std::cout << buffer[view.Offsets + i];
}

/* ============================================================
** URI Parser
** size = last valid index
** ============================================================ */

class UriParser
{
private:
	enum State
	{
		STATE_PATH,
		STATE_QUERY,
		STATE_FRAGMENT,
		STATE_PCT_ENCODED,
		STATE_ERROR
	};

	State		_state;
	State		_pctReturnState;
	uint16_t	_offset;
	bool		_complete;
	s_view		_path;
	s_view		_query;
	s_view		_fragment;
	uint8_t		_pctDigitsRead;

private:
	static bool	isUnreserved(char c)
	{
		unsigned char uc = static_cast<unsigned char>(c);
		return (std::isalnum(uc) || c == '-' || c == '.' || c == '_' || c == '~');
	}

	static bool	isSubDelim(char c)
	{
		return (
			c == '!' || c == '$' || c == '&' || c == '\'' ||
			c == '(' || c == ')' || c == '*' || c == '+' ||
			c == ',' || c == ';' || c == '='
		);
	}

	static bool	isPchar(char c)
	{
		return (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@');
	}

	static bool	isHex(char c)
	{
		return (
			(c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'f') ||
			(c >= 'A' && c <= 'F')
		);
	}

	static bool	isPathChar(char c)
	{
		return (isPchar(c) || c == '/');
	}

	static bool	isQueryChar(char c)
	{
		return (isPchar(c) || c == '/' || c == '?');
	}

	static bool	isFragmentChar(char c)
	{
		return (isPchar(c) || c == '/' || c == '?');
	}

	void	finishPath()
	{
		_path.len = _offset - _path.Offsets;
	}

	void	finishQuery()
	{
		_query.len = _offset - _query.Offsets;
	}

	void	finishFragment()
	{
		_fragment.len = _offset - _fragment.Offsets;
	}

	void	startQuery()
	{
		finishPath();
		_offset++;
		_query.Offsets = _offset;
		_query.len = 0;
		_state = STATE_QUERY;
	}

	void	startFragmentFromPath()
	{
		finishPath();
		_offset++;
		_fragment.Offsets = _offset;
		_fragment.len = 0;
		_state = STATE_FRAGMENT;
	}

	void	startFragmentFromQuery()
	{
		finishQuery();
		_offset++;
		_fragment.Offsets = _offset;
		_fragment.len = 0;
		_state = STATE_FRAGMENT;
	}

	void	enterPctEncoded(State returnState)
	{
		_pctReturnState = returnState;
		_pctDigitsRead = 0;
		_offset++;
		_state = STATE_PCT_ENCODED;
	}

	void	parsePath(const char *buffer, uint16_t size)
	{
		while (_offset <= size)
		{
			char c = buffer[_offset];

			if (c == '?')
			{
				startQuery();
				return;
			}
			if (c == '#')
			{
				startFragmentFromPath();
				return;
			}
			if (c == '%')
			{
				enterPctEncoded(STATE_PATH);
				return;
			}
			if (isSpace(c))
			{
				finishPath();
				_complete = true;
				return;
			}
			if (!isPathChar(c))
			{
				_state = STATE_ERROR;
				return;
			}
			_offset++;
		}
	}

	void	parseQuery(const char *buffer, uint16_t size)
	{
		while (_offset <= size)
		{
			char c = buffer[_offset];

			if (c == '#')
			{
				startFragmentFromQuery();
				return;
			}
			if (c == '%')
			{
				enterPctEncoded(STATE_QUERY);
				return;
			}
			if (isSpace(c))
			{
				finishQuery();
				_complete = true;
				return;
			}
			if (!isQueryChar(c))
			{
				_state = STATE_ERROR;
				return;
			}
			_offset++;
		}
	}

	void	parseFragment(const char *buffer, uint16_t size)
	{
		while (_offset <= size)
		{
			char c = buffer[_offset];

			if (c == '%')
			{
				enterPctEncoded(STATE_FRAGMENT);
				return;
			}
			if (isSpace(c))
			{
				finishFragment();
				_complete = true;
				return;
			}
			if (!isFragmentChar(c))
			{
				_state = STATE_ERROR;
				return;
			}
			_offset++;
		}
	}

	void	parsePctEncoded(const char *buffer, uint16_t size)
	{
		while (_offset <= size && _pctDigitsRead < 2)
		{
			if (!isHex(buffer[_offset]))
			{
				_state = STATE_ERROR;
				return;
			}
			_pctDigitsRead++;
			_offset++;
		}
		if (_pctDigitsRead == 2)
			_state = _pctReturnState;
	}

public:
	UriParser()
	{
		init(0);
	}

	void	init(uint16_t startOffset)
	{
		_state = STATE_PATH;
		_pctReturnState = STATE_PATH;
		_offset = startOffset;
		_complete = false;
		_pctDigitsRead = 0;

		_path.Offsets = startOffset;
		_path.len = 0;
		_query.Offsets = 0;
		_query.len = 0;
		_fragment.Offsets = 0;
		_fragment.len = 0;
	}

	void	parse(const char *buffer, uint16_t size)
	{
		if (_state == STATE_ERROR || _complete)
			return;

		while (_offset <= size && !_complete && _state != STATE_ERROR)
		{
			uint16_t oldOffset = _offset;
			State oldState = _state;

			if (_state == STATE_PATH)
				parsePath(buffer, size);
			else if (_state == STATE_QUERY)
				parseQuery(buffer, size);
			else if (_state == STATE_FRAGMENT)
				parseFragment(buffer, size);
			else if (_state == STATE_PCT_ENCODED)
				parsePctEncoded(buffer, size);

			if (_complete || _state == STATE_ERROR)
				break;
			if (_offset == oldOffset && _state == oldState)
				break;
		}
	}

	bool		isComplete() const { return _complete; }
	bool		isError() const { return (_state == STATE_ERROR); }
	uint16_t	getOffset() const { return _offset; }
	s_view		getPath() const { return _path; }
	s_view		getQuery() const { return _query; }
	s_view		getFragment() const { return _fragment; }
};

/* ============================================================
** Request Line Parser
** size = last valid index
** ============================================================ */

class RequestLine
{
private:
	enum State
	{
		STATE_START,
		STATE_METHOD,
		STATE_URI,
		STATE_VERSION,
		STATE_CR,
		STATE_LF,
		STATE_COMPLETE,
		STATE_ERROR
	};

	enum MethodType
	{
		METHOD_NONE,
		METHOD_GET,
		METHOD_POST,
		METHOD_DELETE
	};

	State		_state;
	MethodType	_methodType;
	uint16_t	_offset;
	bool		_uriReady;
	bool		_versionReady;
	bool		_versionMinorRead;
	s_view		_method;
	s_view		_version;
	uint8_t		_methodIndex;
	uint8_t		_versionIndex;
	UriParser	_uriParser;

private:
	const char	*methodName() const
	{
		if (_methodType == METHOD_GET)
			return "GET";
		if (_methodType == METHOD_POST)
			return "POST";
		if (_methodType == METHOD_DELETE)
			return "DELETE";
		return 0;
	}

	void	selectMethod(const char *buffer, uint16_t size)
	{
<<<<<<< HEAD
<<<<<<< HEAD
		if (_offset > size)
=======
		if (_offset >= size)
>>>>>>> Respond
=======
		if (_offset >= size)
>>>>>>> Request
			return;

		if (buffer[_offset] == 'G')
			_methodType = METHOD_GET;
		else if (buffer[_offset] == 'P')
			_methodType = METHOD_POST;
		else if (buffer[_offset] == 'D')
			_methodType = METHOD_DELETE;
		else
		{
			_state = STATE_ERROR;
			return;
		}

		_method.Offsets = _offset;
		_methodIndex = 0;
		_state = STATE_METHOD;
	}

	void	parseMethod(const char *buffer, uint16_t size)
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
		if (_offset >= size)
			return;
		if (!isSpace(buffer[_offset]))
		{
			_state = STATE_ERROR;
			return;
		}

		_method.len = _offset - _method.Offsets;
		_state = STATE_URI;
	}

	void	parseUri(const char *buffer, uint16_t size)
	{
		if (!_uriReady)
		{
			skipSpaceLastIndex(buffer, size, _offset);
			if (_offset >= size)
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

	void	parseVersion(const char *buffer, uint16_t size)
	{
		const char *expectedPrefix = "HTTP/1.";

		if (!_versionReady)
		{
			skipSpaceLastIndex(buffer, size, _offset);
			if (_offset >= size)
				return;
			_version.Offsets = _offset;
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
<<<<<<< HEAD
<<<<<<< HEAD
			if (_offset > size)
=======
			if (_offset >= size)
>>>>>>> Respond
=======
			if (_offset >= size)
>>>>>>> Request
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

	void	parseCR(const char *buffer, uint16_t size)
	{
		if (_offset >= size)
			return;
		if (buffer[_offset] != '\r')
		{
			_state = STATE_ERROR;
			return;
		}
		_offset++;
		_state = STATE_LF;
	}

	void	parseLF(const char *buffer, uint16_t size)
	{
		if (_offset >= size)
			return;
		if (buffer[_offset] != '\n')
		{
			_state = STATE_ERROR;
			return;
		}
		_offset++;
		_state = STATE_COMPLETE;
	}

public:
	RequestLine()
		: _state(STATE_START),
		  _methodType(METHOD_NONE),
		  _offset(0),
		  _uriReady(false),
		  _versionReady(false),
		  _versionMinorRead(false),
		  _methodIndex(0),
		  _versionIndex(0)
	{
		_method.Offsets = 0;
		_method.len = 0;
		_version.Offsets = 0;
		_version.len = 0;
	}

	void	reset(uint16_t startOffset = 0)
	{
		_state = STATE_START;
		_methodType = METHOD_NONE;
		_offset = startOffset;
		_uriReady = false;
		_versionReady = false;
		_versionMinorRead = false;
		_methodIndex = 0;
		_versionIndex = 0;
		_method.Offsets = startOffset;
		_method.len = 0;
		_version.Offsets = 0;
		_version.len = 0;
		_uriParser.init(startOffset);
	}

	void	parse(const char *buffer, uint16_t size)
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

	bool		isComplete() const { return (_state == STATE_COMPLETE); }
	bool		isError() const { return (_state == STATE_ERROR); }
	s_view		getMethod() const { return _method; }
	s_view		getVersion() const { return _version; }
	UriParser	getRequestURI() const { return _uriParser; }
	uint16_t	getOffset() const { return _offset; }
};

/* ============================================================
** Header Parser
** size = last valid index
** ============================================================ */

class Header
{
private:
	#define SEED 0x12345678

	enum State
	{
		STATE_KEY,
		STATE_VALUE,
		STATE_CR,
		STATE_LF,
		STATE_DECISION,
		STATE_COMPLETE,
		STATE_ERROR
	};

	stPollRequest				&_request;
	uint8_t						_state;
	uint16_t					_offset;
	uint16_t					_keyStart;
	uint16_t					_valueStart;
	uint32_t					_hash;
	bool						emptyLinePending;
	bool						skipSpaceAfterColon;
	HttpTables::eKnownHeader	_currentHeader;
	uint8_t						_currentUnknownIndex;
	uint8_t						_indexUnknownHeaders;

private:
	void	hashStep(char c)
	{
		_hash ^= static_cast<unsigned char>(c);
		_hash *= 0x5bd1e995;
		_hash ^= _hash >> 15;
	}

	HttpTables::eKnownHeader fromHash(uint32_t h)
	{
		switch (h)
		{
			case 0x1e5aeec4: return HttpTables::H_HOST;
			case 0x240bbc50: return HttpTables::H_CONTENT_LENGTH;
			case 0x41141d0c: return HttpTables::H_TRANSFER_ENCODING;
			case 0xc947e5d0: return HttpTables::H_CONTENT_TYPE;
			case 0x8fd8a976: return HttpTables::H_CONNECTION;
			case 0x8210d858: return HttpTables::H_EXPECT;
			case 0x60f97fde: return HttpTables::H_AUTHORIZATION;
			case 0x4def559a: return HttpTables::H_COOKIE;
			case 0xa13e549b: return HttpTables::H_ACCEPT_ENCODING;
			default: return HttpTables::H_UNKNOWN;
		}
	}


	bool	isHeaderKeyChar(char c)
	{
		unsigned char uc = static_cast<unsigned char>(c);

		if (std::isalnum(uc))
			return true;

		switch (c)
		{
			case '!':
			case '#':
			case '$':
			case '%':
			case '&':
			case '\'':
			case '*':
			case '+':
			case '-':
			case '.':
			case '^':
			case '_':
			case '`':
			case '|':
			case '~':
				return true;
			default:
				return false;
		}
	}

	bool	isHeaderValueChar(char c)
	{
		unsigned char uc = static_cast<unsigned char>(c);
		return (uc >= 32 && uc <= 126);
	}

	bool	canRead(uint16_t size) const
	{
		return (_offset <= size);
	}

	void	makeUnknownHeader()
	{
		if (_indexUnknownHeaders >= _request.sizeUnknownHeaders)
		{
			_state = STATE_ERROR;
			return;
		}

		_currentUnknownIndex = _indexUnknownHeaders;
		_request.unknown_headers[_currentUnknownIndex].Hash = _hash;
		_request.unknown_headers[_currentUnknownIndex].key.Offsets = _keyStart;
		_request.unknown_headers[_currentUnknownIndex].key.len = (_offset - 1) - _keyStart;
		_request.unknown_headers[_currentUnknownIndex].next = INVALID_INDEX;
	}

	void	makeKnownHeader()
	{
		if (_request.known_headers[_currentHeader].key.Offsets == INVALID_INDEX)
		{
			_request.known_headers[_currentHeader].Hash = _hash;
			_request.known_headers[_currentHeader].key.Offsets = _keyStart;
			_request.known_headers[_currentHeader].key.len = (_offset - 1) - _keyStart;
		}
		else
		{
			makeUnknownHeader();
			if (_state == STATE_ERROR)
				return;

			uint8_t next = _request.known_headers[_currentHeader].next;
			if (next == INVALID_INDEX)
			{
				_request.known_headers[_currentHeader].next = _currentUnknownIndex;
			}
			else
			{
				while (_request.unknown_headers[next].next != INVALID_INDEX)
					next = _request.unknown_headers[next].next;
				_request.unknown_headers[next].next = _currentUnknownIndex;
			}
		}
	}

	void	selectHeaderSlot()
	{
		_currentHeader = fromHash(_hash);
		_currentUnknownIndex = INVALID_INDEX;

		if (_currentHeader != HttpTables::H_UNKNOWN)
			makeKnownHeader();
		else
			makeUnknownHeader();

		if (_state == STATE_ERROR)
			return;

		_hash = SEED;
		skipSpaceAfterColon = true;
	}

	void	storeValue()
	{
		uint16_t valueLen = _offset - _valueStart;

		if (_currentHeader != HttpTables::H_UNKNOWN &&
			_currentUnknownIndex == INVALID_INDEX)
		{
			_request.known_headers[_currentHeader].val.Offsets = _valueStart;
			_request.known_headers[_currentHeader].val.len = valueLen;
		}
		else
		{
			_request.unknown_headers[_currentUnknownIndex].val.Offsets = _valueStart;
			_request.unknown_headers[_currentUnknownIndex].val.len = valueLen;
			_indexUnknownHeaders++;
		}
	}

	void	parseKey(uint16_t size)
	{
		while (canRead(size) && _state == STATE_KEY)
		{
			char c = _request.request_metadata[_offset];

			if (c == ':')
			{
				_offset++;
				selectHeaderSlot();
				if (_state == STATE_ERROR)
					return;
				_state = STATE_VALUE;
				return;
			}

			if (!isHeaderKeyChar(c))
			{
				_state = STATE_ERROR;
				return;
			}

			hashStep(tolower(c));
			_offset++;
		}
	}

	void	parseValue(uint16_t size)
	{
		if (skipSpaceAfterColon)
		{
			skipSpaceLastIndex(_request.request_metadata, size, _offset);
			if (!canRead(size))
				return;
			_valueStart = _offset;
			skipSpaceAfterColon = false;
		}

		while (canRead(size) && _state == STATE_VALUE)
		{
			char c = _request.request_metadata[_offset];

			if (c == '\r')
			{
				storeValue();
				_state = STATE_CR;
				return;
			}

			if (!isHeaderValueChar(c))
			{
				_state = STATE_ERROR;
				return;
			}

			_offset++;
		}
	}

	void	parseCR(uint16_t size)
	{
		if (!canRead(size))
			return;

		if (_request.request_metadata[_offset] != '\r')
		{
			_state = STATE_ERROR;
			return;
		}

		_offset++;
		_state = STATE_LF;
	}

	void	parseLF(uint16_t size)
	{
		if (!canRead(size))
			return;

		if (_request.request_metadata[_offset] != '\n')
		{
			_state = STATE_ERROR;
			return;
		}

		_offset++;
		_state = STATE_DECISION;
	}

	void	parseDecision(uint16_t size)
	{
		if (!canRead(size))
			return;

		if (_request.request_metadata[_offset] == '\r')
		{
			emptyLinePending = true;
			_state = STATE_CR;
			return;
		}

		emptyLinePending = false;
		_keyStart = _offset;
		_hash = SEED;
		_currentHeader = HttpTables::H_UNKNOWN;
		_currentUnknownIndex = INVALID_INDEX;
		_state = STATE_KEY;
	}

public:
	Header(stPollRequest &request) : _request(request)
	{
		_state = STATE_KEY;
		_offset = 0;
		_keyStart = 0;
		_valueStart = 0;
		_hash = SEED;
		emptyLinePending = false;
		skipSpaceAfterColon = false;
		_currentHeader = HttpTables::H_UNKNOWN;
		_currentUnknownIndex = INVALID_INDEX;
		_indexUnknownHeaders = 0;
	}

	void	init(uint16_t offset)
	{
		_offset = offset;
		_keyStart = offset;
		_valueStart = offset;
		_hash = SEED;
		emptyLinePending = false;
		skipSpaceAfterColon = false;
		_currentHeader = HttpTables::H_UNKNOWN;
		_currentUnknownIndex = INVALID_INDEX;
		_indexUnknownHeaders = 0;
		_state = STATE_KEY;
	}

	void	Parse(uint16_t size)
	{
		while (canRead(size))
		{
			uint16_t oldOffset = _offset;
			uint8_t oldState = _state;

			if (_state == STATE_KEY)
				parseKey(size);
			else if (_state == STATE_VALUE)
				parseValue(size);
			else if (_state == STATE_CR)
				parseCR(size);
			else if (_state == STATE_LF)
			{
				parseLF(size);
				if (_state == STATE_DECISION && emptyLinePending)
					_state = STATE_COMPLETE;
			}
			else if (_state == STATE_DECISION)
				parseDecision(size);
			else
				break;

			if (_state == STATE_ERROR || _state == STATE_COMPLETE)
				break;
			if (_offset == oldOffset && _state == oldState)
				break;
		}
	}

	uint16_t	getOffset() const { return _offset; }
	bool		isError() const { return (_state == STATE_ERROR); }
	bool		isComplete() const { return (_state == STATE_COMPLETE); }
};

/* ============================================================
** High-level Request Parser
** size = last valid index
** ============================================================ */

class RequestParser
{
private:
	enum State
	{
		STATE_REQUEST_LINE,
		STATE_HEADERS,
		STATE_BODY,
		STATE_COMPLETE,
		STATE_ERROR
	};

	stPollRequest	&_request;
	State			_state;
	uint16_t		_offset;
	RequestLine		_requestLine;
	Header			_header;

	void	ParseRequestLine(uint16_t size)
	{
		_requestLine.parse(_request.request_metadata, size);
		_offset = _requestLine.getOffset();

		if (_requestLine.isError())
		{
			_state = STATE_ERROR;
			return;
		}
		if (_requestLine.isComplete())
		{
			_state = STATE_HEADERS;
			_header.init(_offset);
		}
	}

	void	ParseHeader(uint16_t size)
	{
		_header.Parse(size);
		_offset = _header.getOffset();

		if (_header.isError())
		{
			_state = STATE_ERROR;
			return;
		}
		if (_header.isComplete())
		{

			_state = STATE_COMPLETE;
		}
	}

	void	ParseBody(uint16_t)
	{
		_state = STATE_COMPLETE;
	}

public:
	RequestParser(stPollRequest &request)
		: _request(request),
		  _state(STATE_REQUEST_LINE),
		  _offset(0),
		  _requestLine(),
		  _header(request)
	{
	}

	void	init(uint16_t offset = 0)
	{
		_state = STATE_REQUEST_LINE;
		_offset = offset;
		_requestLine.reset(offset);
		_header.init(offset);
	}

	void	Parse(uint16_t size)
	{
		while (_offset <= size)
		{
			uint16_t oldOffset = _offset;
			State oldState = _state;

			if (_state == STATE_REQUEST_LINE)
				ParseRequestLine(size);
			else if (_state == STATE_HEADERS)
				ParseHeader(size);
			else if (_state == STATE_BODY)
				ParseBody(size);
			else
				break;

			if (_state == STATE_COMPLETE || _state == STATE_ERROR)
				break;
			if (_offset == oldOffset && _state == oldState)
				break;
		}
	}

	bool isComplete() const { return (_state == STATE_COMPLETE); }
	bool isError() const { return (_state == STATE_ERROR); }
	RequestLine getRequestLine() const { return _requestLine; }
};

/* ============================================================
** Demo
** ============================================================ */



// #include "ProcessRequestHandler.hpp"




// ProcessRequestHandler::ProcessRequestHandler()
// {
// }


// const clsLocation* findBestLocation(
// 	const std::vector<clsLocation>		&LocationExact,
// 	const std::vector<clsLocation>		&LocationPrefix,
// 	s_view &uri, const char *buffer)
// {
// 	const clsLocation* best = NULL;

// 	if (LocationExact.size() > 0)
// 	{
// 		for (size_t i = 0; i < LocationExact.size(); i++)
// 		{
// 			if (uri.len == LocationExact[i].getLocationData().uri.size())
// 				if (!compare_view(uri, buffer, LocationExact[i].getLocationData().uri))
// 					return &LocationExact[i];
// 		}
// 	}

// 	size_t maxLen = 0;

// 	if (LocationPrefix.size() > 0)
// 	{
// 		const std::vector<clsLocation>& prefixVec = LocationPrefix;

// 		for (size_t i = 0; i < prefixVec.size(); i++)
// 		{
// 			const std::string& loc = prefixVec[i].getLocationData().uri;

// 			int com = compare_view(uri ,buffer, loc);
// 			if (com == 1 || !com)
// 			{
// 				if (loc.size() > maxLen)
// 				{
// 					maxLen = loc.size();
// 					best = &prefixVec[i];
// 				}
// 			}
// 		}
// 	}

// 	return best;
// }

// const std::string ProcessRequestHandler::getPathCgi(const std::string &uri, const std::map<std::string, std::string> &cgi_pass)
// {
// 	const std::string empty = "";
// 	size_t extension = uri.find_last_of('.');
// 	if (extension == std::string::npos)
// 		return empty;
// 	std::string extensionStr = uri.substr(extension);
// 	std::map<std::string, std::string>::const_iterator it = cgi_pass.find(extensionStr);

// 	if (it != cgi_pass.end())
// 		return it->second;
// 	return empty;
// }

// std::string ProcessRequestHandler::selectMethod(Methods::eMethods method) {
// 	switch (method) {
// 		case Methods::GET:
// 			return "GET";
// 		case Methods::POST:
// 			return "POST";
// 		case Methods::DELETE:
// 			return "DELETE";
// 		default:
// 			return "UNKNOWN";
// 	}
// }



// bool    checkPath(const std::string &physicalPath)
// {
// 	struct stat buffer;
	
// 	if (!stat(physicalPath.c_str(), &buffer))
// 	{
// 		if (!access(physicalPath.c_str(), R_OK))
// 			return (true);
// 	}
// 	return (false);
// }

// std::string ProcessRequestHandler::handleDirectory(const clsLocation* bestLocation, HttpError &error)
// {
// 	const std::vector<std::string> &vindex = bestLocation->getIndex();
// 	std::string rootOrAlias = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
// 	std::string physicalPath = "";

// 	for (size_t i = 0; i < vindex.size(); i++)
// 	{
// 		physicalPath = rootOrAlias + "/" + vindex[i];
// 		if (checkPath(physicalPath))
// 			return physicalPath;
// 	}

// 	physicalPath = rootOrAlias + "/index.html";
// 	if (checkPath(physicalPath))
// 		return physicalPath;

// 	if (bestLocation->getAutoIndex())
// 		return rootOrAlias;

// 	error.setStatus(403, "Forbidden");
// 	return "";
// }

// std::string ProcessRequestHandler::creatPhysicalPath(const clsLocation* bestLocation, const std::string &uri, HttpError &error)
// {
// 	std::string base = "";
	
// 	if (!uri.empty() && uri[uri.size() - 1] == '/')
// 		return handleDirectory(bestLocation, error);

// 	base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();

// 	std::string physicalPath;
// 	if (!bestLocation->getAlias().empty()) {
// 		std::string subUri = uri.substr(bestLocation->getLocationData().uri.size());
// 		physicalPath = base + ( (!subUri.empty() && subUri[0] != '/') ? "/" + subUri : subUri );
// 	}
// 	else
// 		physicalPath = base + ( (!uri.empty() && uri[0] != '/') ? "/" + uri : uri );

// 	if (checkPath(physicalPath))
// 		return physicalPath;

// 	error.setStatus(404, "Not Found");
// 	return "";
// }

// void ProcessRequestHandler::processRequest(const RequestParser& request, const clsServerConfig& serverConfigs, RequestHandler& handler)
// {
// 	const clsLocation* bestLocation = findBestLocation(serverConfigs.getLocationExact(), serverConfigs.getLocationPrefix(), request._startLine.getPath());
// 	HttpError error;

// 	if (bestLocation)
// 	{
// 		handler.setPhysicalPath(creatPhysicalPath(bestLocation, request.getRequestLine().getRequestURI(), error));
// 		handler.setAutoIndex(bestLocation->getAutoIndex());
// 		handler.setAllowMethod(request._startLine.getMethod() == (bestLocation->getAllowMethods() & request._startLine.getMethod()));
// 		handler.setQuery(request._startLine.getQuery());
// 		handler.setVersion(request._startLine.getVersion());
// 		handler.setMethod(selectMethod(request._startLine.getMethod()));
// 		handler.setHeaders(request._headerParser.getHeaderValues());
// 		handler.setErrorPages(bestLocation->getErrorPages());
// 		handler.setPathCgi(getPathCgi(request._startLine.getPath(), bestLocation->getCgiPass()));
// 		handler.setReturn(bestLocation->getReturn());
// 		handler.setUploadStore(bestLocation->getUploadStore());
// 		handler.setError(error);
// 	}
	
// }

int main(void)
{
	PollOfClient client;

	const char *raw =
	"GET /api/users/123 HTTP/1.1\r\n"
	"Host: api.example.com\r\n"
	"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\r\n"
	"Content-Length: 20\r\n"
	"Accept: application/json\r\n"
	"Accept-Language: en-US,en;q=0.9\r\n"
	"\r\n";

	memcpy(client.request_metadata, raw, strlen(raw) + 1);

	stPollRequest req = makeRequest(client);
	RequestParser parser(req);
	parser.init(0);

	for (int i = 0; client.request_metadata[i]; i++)
	{
		parser.Parse(i);
		if (parser.isComplete() || parser.isError())
			break ;
	}

	if (parser.isError())
	{
		std::cout << "parse error\n";
		return 1;
	}

	if (parser.isComplete())
	{
		std::cout << "Method: ";
		print_view(client.request_metadata, parser.getRequestLine().getMethod());
		std::cout << "\n";

		std::cout << "Path: ";
		print_view(client.request_metadata, parser.getRequestLine().getRequestURI().getPath());
		std::cout << "\n";

		std::cout << "Version: ";
		print_view(client.request_metadata, parser.getRequestLine().getVersion());
		std::cout << "\n";

		std::cout << "Host key: ";
		print_view(client.request_metadata, req.known_headers[HttpTables::H_HOST].key);
		std::cout << "\n";

		std::cout << "Host value: ";
		print_view(client.request_metadata, req.known_headers[HttpTables::H_HOST].val);
		std::cout << "\n";
	}

	return 0;
}