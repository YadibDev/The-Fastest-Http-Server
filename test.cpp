#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>

struct s_view
{
	uint16_t Offsets;
	uint16_t len;

	s_view() : Offsets(0), len(0) {}
};

struct s_header_slot
{
	s_view	key;
	s_view	val;
	uint8_t	next;
	s_header_slot() : next(255) {}
};

struct stPollRequest
{
	char			*request_metadata;
	s_header_slot	*known_headers;
	s_header_slot	*unknown_headers;
	char			*io_chunk;
	short			sizeUnknownHeaders;
};

void skipSpace(char *str, uint16_t size, uint16_t &index)
{
	while (index < size && (str[index] == ' ' || str[index] == '\t'))
		index++;
}

class URIParser
{
	enum eSTATE
	{
		PATH,
		QUERY,
		FRAGMENT,
		PCT_ENCODED,
		ERROR
	};

	uint8_t		state;
	uint8_t		pct_return_state;
	uint16_t	offset;
	bool		convertState;
	char		pct_buffer[2];
	uint8_t		pct_index;

	s_view		_path;
	s_view		_query;
	s_view		_fragment;

	static bool isUnreserved(char c)
	{
		return (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '.' || c == '_' || c == '~');
	}

	static bool isSubDelim(char c)
	{
		return (c == '!' || c == '$' || c == '&' || c == '\'' ||
				c == '(' || c == ')' || c == '*' || c == '+' ||
				c == ',' || c == ';' || c == '=');
	}

	static bool isPchar(char c)
	{
		return (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@');
	}

	static bool isHex(char c)
	{
		return ((c >= '0' && c <= '9') ||
				(c >= 'a' && c <= 'f') ||
				(c >= 'A' && c <= 'F'));
	}

	void enterPctState(uint8_t return_state)
	{
		pct_buffer[0] = 0;
		pct_buffer[1] = 0;
		pct_index = 0;
		pct_return_state = return_state;
		state = PCT_ENCODED;
		offset++;
	}

	void parsePath(char *buf, uint16_t len)
	{
		while (offset < len)
		{
			char c = buf[offset];

			if (c == '?')
			{
				_query.Offsets = offset;
				_query.len = 0;
				state = QUERY;
			}
			else if (c == '#')
			{	
				_fragment.Offsets = offset;
				_fragment.len = 0;
				state = FRAGMENT;
			}
			if (state != PATH)
			{
				_path.len = offset - _path.Offsets;
				offset++;
			}
			if (c == '%')
			{
				enterPctState(PATH);
				return;
			}
			if (c == ' ' || c == '\t')
			{
				_path.len = offset - _path.Offsets;
				convertState = true;
				return;
			}
			if (!isPchar(c) && c != '/')
			{
				state = ERROR;
				return;
			}
			offset++;
		}
	}

	void parseQuery(char *buf, uint16_t len)
	{
		while (offset < len)
		{
			char c = buf[offset];

			if (c == '#')
			{
				_query.len = offset - _query.Offsets;
				offset++;
				_fragment.Offsets = offset;
				_fragment.len = 0;
				state = FRAGMENT;
				return;
			}
			if (c == '%')
			{
				enterPctState(QUERY);
				return;
			}
			if (c == ' ' || c == '\t')
			{
				_query.len = offset - _query.Offsets;
				convertState = true;
				return;
			}
			if (!isPchar(c) && c != '/' && c != '?')
			{
				state = ERROR;
				return;
			}
			offset++;
		}
	}

	void parseFragment(char *buf, uint16_t len)
	{
		while (offset < len)
		{
			char c = buf[offset];

			if (c == '%')
			{
				enterPctState(FRAGMENT);
				return;
			}
			if (c == ' ' || c == '\t')
			{
				_fragment.len = offset - _fragment.Offsets;
				convertState = true;
				return;
			}
			if (!isPchar(c) && c != '/' && c != '?')
			{
				state = ERROR;
				return;
			}
			offset++;
		}
	}

	void parsePctEncoded(char *buf, uint16_t len)
	{
		while (offset < len && pct_index < 2)
		{
			char c = buf[offset];
			if (!isHex(c))
			{
				state = ERROR;
				return;
			}
			pct_buffer[pct_index++] = c;
			offset++;
		}
		if (pct_index == 2)
			state = pct_return_state;
	}

public:
	URIParser() : state(PATH), pct_return_state(PATH), offset(0), convertState(false), pct_index(0)
	{
		pct_buffer[0] = 0;
		pct_buffer[1] = 0;
	}

	void init(uint16_t startOffset)
	{
		offset = startOffset;
		_path.Offsets = startOffset;
		state = PATH;
		pct_return_state = PATH;
		convertState = false;
		pct_buffer[0] = 0;
		pct_buffer[1] = 0;
		pct_index = 0;
	}

	void parse(char *buf, uint16_t len)
	{
		if (state == ERROR || convertState)
			return;

		while (offset < len && !convertState && state != ERROR)
		{
			if (state == PATH)
				parsePath(buf, len);
			else if (state == QUERY)
				parseQuery(buf, len);
			else if (state == FRAGMENT)
				parseFragment(buf, len);
			else if (state == PCT_ENCODED)
				parsePctEncoded(buf, len);
		}
	}

	bool isComplete() const { return (convertState && state != ERROR); }
	bool isError() const { return (state == ERROR); }
	uint16_t getOffset() const { return offset; }
	s_view getPath() const { return _path; }
	s_view getQuery() const { return _query; }
	s_view getFragment() const { return _fragment; }
};

class RequestLine
{
	enum eSTATE
	{
		START,
		GET,
		POST,
		DELETE,
		PARSE_REQUEST_URI,
		PARSE_VERSION,
		CRLF,
		COMPLETE,
		SP,
		ERROR
	};

	s_view		_Method;
	s_view		_Version;
	URIParser	_URIParser;

	uint16_t	_Offset_Parse;
	uint8_t		_STATE;
	uint8_t		_method_index;
	uint8_t		_version_index;
	bool		convertState;
	bool		_waiting_lf;

	const char *getMethodString() const
	{
		if (_STATE == GET)
			return "GET";
		if (_STATE == POST)
			return "POST";
		if (_STATE == DELETE)
			return "DELETE";
		return 0;
	}

	void SelectMethod(char *Buffer, uint16_t len)
	{
		if (_Offset_Parse >= len)
			return;
		switch (Buffer[_Offset_Parse])
		{
			case 'G': _STATE = GET; break;
			case 'P': _STATE = POST; break;
			case 'D': _STATE = DELETE; break;
			default: _STATE = ERROR; break;
		}
		_Method.Offsets = _Offset_Parse;
		_method_index = 0;
	}

	void ParseMethod(char *buffer, uint16_t len)
	{
		const char *method = getMethodString();

		if (!method)
		{
			_STATE = ERROR;
			return;
		}
		while (method[_method_index] && _Offset_Parse < len)
		{
			if (buffer[_Offset_Parse] != method[_method_index])
			{
				_STATE = ERROR;
				return;
			}
			_Offset_Parse++;
			_method_index++;
		}
		if (method[_method_index] == '\0')
		{
			if (_Offset_Parse >= len)
				return;
			if (buffer[_Offset_Parse] != ' ' && buffer[_Offset_Parse] != '\t')
			{
				_STATE = ERROR;
				return;
			}
			_Method.len = _Offset_Parse - _Method.Offsets;
			convertState = true;
		}
	}

	void ParseRequestUri(char *buffer, uint16_t len)
	{
		convertState = false;
		_URIParser.parse(buffer, len);
		if (_URIParser.isError())
		{
			_STATE = ERROR;
			return;
		}
		if (_URIParser.isComplete())
		{
			convertState = true;
			_Offset_Parse = _URIParser.getOffset();
		}
	}

	void ParseVersion(char *buffer, uint16_t len)
	{
		const char *prefix = "HTTP/1.";

		while (_version_index < 7 && _Offset_Parse < len)
		{
			if (buffer[_Offset_Parse] != prefix[_version_index])
			{
				_STATE = ERROR;
				return;
			}
			_Offset_Parse++;
			_version_index++;
		}
		if (_version_index < 7)
			return;

		if (_version_index == 7)
		{
			if (_Offset_Parse >= len)
				return;
			if (buffer[_Offset_Parse] != '0' && buffer[_Offset_Parse] != '1')
			{
				_STATE = ERROR;
				return;
			}
			_Offset_Parse++;
			_version_index++;
			_Version.len = 8;
		}

		if (_version_index == 8)
			_STATE = CRLF;
	}

	void ParseCRLF(char *buffer, uint16_t len)
	{
		if (!_waiting_lf)
		{
			if (_Offset_Parse >= len)
				return;
			if (buffer[_Offset_Parse] != '\r')
			{
				_STATE = ERROR;
				return;
			}
			_Offset_Parse++;
			_waiting_lf = true;
		}
		if (_waiting_lf)
		{
			if (_Offset_Parse >= len)
				return;
			if (buffer[_Offset_Parse] != '\n')
			{
				_STATE = ERROR;
				return;
			}
			_Offset_Parse++;
			convertState = true;
			_STATE = COMPLETE;
		}
	}

public:
	RequestLine(uint16_t Offset_Parse)
		: _Offset_Parse(Offset_Parse),
		  _STATE(START),
		  convertState(false),
		  _method_index(0),
		  _version_index(0),
		  _waiting_lf(false)
	{
		_Method.Offsets = Offset_Parse;
		_Method.len = 0;
		_Version.Offsets = 0;
		_Version.len = 0;
	}

	void Parse(char *Buffer, uint16_t len)
	{
		if (_STATE == COMPLETE || _STATE == ERROR)
			return;

		convertState = false;

		while (_Offset_Parse < len && _STATE != COMPLETE && _STATE != ERROR)
		{
			if (_STATE == START)
			{
				SelectMethod(Buffer, len);
			}
			else if (_STATE == GET || _STATE == POST || _STATE == DELETE)
			{
				ParseMethod(Buffer, len);
				if (convertState)
				{
					skipSpace(Buffer, len, _Offset_Parse);
					if (Buffer[_Offset_Parse] == ' ' || Buffer[_Offset_Parse] == '\t')
						return ;
					_STATE = PARSE_REQUEST_URI;
					_URIParser.init(_Offset_Parse);
					convertState = false;
				}
				else
					break;
			}
			else if (_STATE == PARSE_REQUEST_URI)
			{
				ParseRequestUri(Buffer, len);
				if (_STATE == ERROR)
					return;
				if (convertState)
				{
					skipSpace(Buffer, len, _Offset_Parse);
					if (Buffer[_Offset_Parse] == ' ' || Buffer[_Offset_Parse] == '\t')
						return ;
					_STATE = PARSE_VERSION;
					_Version.Offsets = _Offset_Parse;
					_version_index = 0;
					convertState = false;
				}
				else
					break;
			}
			else if (_STATE == PARSE_VERSION)
			{
				ParseVersion(Buffer, len);
				if (_STATE == ERROR)
					return;
				if (_STATE != CRLF)
					break;
			}
			else if (_STATE == CRLF)
			{
				ParseCRLF(Buffer, len);
				if (_STATE == ERROR || _STATE == COMPLETE)
					return;
				break;
			}
		}
	}

	s_view getMethod() const { return _Method; }
	s_view getVersion() const { return _Version; }
	URIParser getRequestURI() const { return _URIParser; }
	bool isComplete() const { return (_STATE == COMPLETE); }
	bool isError() const { return (_STATE == ERROR); }
};

class RequestParser
{
	enum eSTATE
	{
		START,
		STATE_REQUEST_LINE,
		STATE_HEADER,
		STATE_BODY,
		COMPLETE
	};

	stPollRequest	_PollRequest;
	eSTATE			_STATE;
	uint16_t		_Offset_Parse;

public:
	RequestParser(stPollRequest &PollRequest)
		: _PollRequest(PollRequest), _STATE(START), _Offset_Parse(0)
	{
	}
};




#include <cctype>
#include <stdint.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

/* ── paste your structs + URIParser + RequestLine here ── */
/* ... (s_view, s_header_slot, stPollRequest, isSpace, skipSpace, URIParser, RequestLine) */

/* ================================================================
** TEST FRAMEWORK
** ================================================================ */

static int g_pass = 0;
static int g_fail = 0;

static void result(const char *name, bool ok)
{
	if (ok)
	{
		printf("  \033[32m[PASS]\033[0m %s\n", name);
		g_pass++;
	}
	else
	{
		printf("  \033[31m[FAIL]\033[0m %s\n", name);
		g_fail++;
	}
}

/* Helper: feed the entire buffer in ONE shot */
static void feedAll(RequestLine &rl, char *data)
{
	rl.Parse(data, (uint16_t)strlen(data));
}

/* Helper: feed the buffer byte-by-byte (hardest chunking) */
static void feedByByte(RequestLine &rl, char *data)
{
	uint16_t len = (uint16_t)strlen(data);
	for (uint16_t i = 1; i <= len; i++)
		rl.Parse(data, i);   /* cumulative view, same as a stream */
}

/* Helper: feed in arbitrary chunks defined by split positions */
static void feedChunks(RequestLine &rl, char *data,
                       const std::vector<uint16_t> &cuts)
{
	/* cuts = list of end-offsets for each chunk */
	for (size_t i = 0; i < cuts.size(); ++i)
	{
		uint16_t cut = cuts[i];
		rl.Parse(data, cut);   /* simulate incremental buffer growth */
		if (rl.isComplete() || rl.isError())
			return;
	}
	rl.Parse(data, (uint16_t)strlen(data));
}

/* Helper: compare s_view against expected string */
static bool viewEq(const char *buffer, s_view v, const char *expected)
{
	size_t elen = strlen(expected);
	if (v.len != elen)
		return false;
	return (memcmp(buffer + v.Offsets, expected, elen) == 0);
}

/* ================================================================
** TESTS
** ================================================================ */

/* ── GROUP 1: VALID – single-shot ── */

static void t01_get_simple()
{
	char *buf = "GET / HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("01 GET / HTTP/1.1 (single-shot)",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getMethod(), "GET") &&
		viewEq(buf, rl.getRequestURI().getPath(), "/") &&
		viewEq(buf, rl.getVersion(), "HTTP/1.1"));
}

static void t02_post_with_path()
{
	 char *buf = "POST /submit/form HTTP/1.0\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("02 POST /submit/form HTTP/1.0",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getMethod(), "POST") &&
		viewEq(buf, rl.getRequestURI().getPath(), "/submit/form") &&
		viewEq(buf, rl.getVersion(), "HTTP/1.0"));
}

static void t03_delete_root()
{
	 char *buf = "DELETE /res/42 HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("03 DELETE /res/42 HTTP/1.1",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getMethod(), "DELETE"));
}

static void t04_query_string()
{
	 char *buf = "GET /search?q=hello&lang=en HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("04 Query string parsed",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getPath(), "/search") &&
		viewEq(buf, rl.getRequestURI().getQuery(), "q=hello&lang=en"));
}

static void t05_fragment()
{
	 char *buf = "GET /page#section2 HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("05 Fragment parsed",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getFragment(), "section2"));
}

static void t06_pct_encoded_path()
{
	 char *buf = "GET /hello%20world HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("06 Percent-encoded path",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getPath(), "/hello%20world"));
}

static void t07_full_uri_all_parts()
{
	 char *buf = "GET /p/a?x=1#frag HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("07 Path + query + fragment",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getPath(), "/p/a") &&
		viewEq(buf, rl.getRequestURI().getQuery(), "x=1") &&
		viewEq(buf, rl.getRequestURI().getFragment(), "frag"));
}

/* ── GROUP 2: MULTIPLE SPACES / TABS ── */

static void t08_multiple_spaces_before_uri()
{
	 char *buf = "GET        / HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("08 Multiple spaces before URI",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getPath(), "/"));
}

static void t09_tab_before_uri()
{
	 char *buf = "GET\t/path HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("09 Tab between method and URI",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getPath(), "/path"));
}

static void t10_mixed_spaces_tabs()
{
	 char *buf = "GET \t \t /mix HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("10 Mixed spaces+tabs before URI",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getPath(), "/mix"));
}

static void t11_multiple_spaces_before_version()
{
	 char *buf = "GET /x        HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("11 Multiple spaces before version",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getVersion(), "HTTP/1.1"));
}

static void t12_tabs_before_version()
{
	 char *buf = "GET /x\t\t\tHTTP/1.0\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("12 Tabs before version",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getVersion(), "HTTP/1.0"));
}

static void t13_spaces_both_sides()
{
	 char *buf = "GET    /both   HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("13 Spaces on both sides of URI",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getPath(), "/both") &&
		viewEq(buf, rl.getVersion(), "HTTP/1.1"));
}

/* ── GROUP 3: CHUNKED DELIVERY ── */

static void t14_byte_by_byte_get()
{
	 char *buf = "GET / HTTP/1.1\r\n";
	RequestLine rl(0);
	feedByByte(rl, buf);
	result("14 Byte-by-byte GET /",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getMethod(), "GET") &&
		viewEq(buf, rl.getRequestURI().getPath(), "/") &&
		viewEq(buf, rl.getVersion(), "HTTP/1.1"));
}

static void t15_chunk_split_in_spaces()
{
	/* split right in the middle of the spaces */
	 char *buf = "GET        /path HTTP/1.1\r\n";
	RequestLine rl(0);
	/* chunk1 = "GET   " (6 chars), chunk2 = rest */
	std::vector<uint16_t> cuts;
	cuts.push_back(6);
	cuts.push_back(12);
	cuts.push_back((uint16_t)strlen(buf));
	feedChunks(rl, buf, cuts);
	result("15 Chunk split inside spaces before URI",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getRequestURI().getPath(), "/path"));
}

static void t16_chunk_split_in_method()
{
	 char *buf = "DELETE /r HTTP/1.1\r\n";
	RequestLine rl(0);
	/* split after "DE" */
	std::vector<uint16_t> cuts;
	cuts.push_back(2);
	cuts.push_back(5);
	cuts.push_back((uint16_t)strlen(buf));
	feedChunks(rl, buf, cuts);
	result("16 Chunk split mid-method (DE|LETE)",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getMethod(), "DELETE"));
}

static void t17_chunk_split_in_version()
{
	 char *buf = "GET / HTTP/1.1\r\n";
	RequestLine rl(0);
	/* split inside "HTTP/1" */
	uint16_t vstart = (uint16_t)(strchr(buf, 'H') - buf);
	std::vector<uint16_t> cuts;
	cuts.push_back((uint16_t)(vstart + 3));
	cuts.push_back((uint16_t)strlen(buf));
	feedChunks(rl, buf, cuts);
	result("17 Chunk split mid-version (HTT|P/1.1)",
		rl.isComplete() && !rl.isError());
}

static void t18_chunk_split_at_crlf()
{
	 char *buf = "GET / HTTP/1.1\r\n";
	RequestLine rl(0);
	uint16_t len = (uint16_t)strlen(buf);
	/* give everything except the \n */
	std::vector<uint16_t> cuts;
	cuts.push_back((uint16_t)(len - 1));
	cuts.push_back(len);
	feedChunks(rl, buf, cuts);
	result("18 Chunk split between CR and LF",
		rl.isComplete() && !rl.isError());
}

static void t19_chunk_split_spaces_before_version()
{
	 char *buf = "GET /x     HTTP/1.1\r\n";
	RequestLine rl(0);
	/* split right after /x, inside trailing spaces */
	std::vector<uint16_t> cuts;
	cuts.push_back(7);
	cuts.push_back(10);
	cuts.push_back((uint16_t)strlen(buf));
	feedChunks(rl, buf, cuts);
	result("19 Chunk split inside spaces before version",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getVersion(), "HTTP/1.1"));
}

static void t20_single_byte_chunks_with_many_spaces()
{
	 char *buf = "POST   /a   HTTP/1.0\r\n";
	RequestLine rl(0);
	feedByByte(rl, buf);
	result("20 Byte-by-byte with multiple spaces",
		rl.isComplete() && !rl.isError() &&
		viewEq(buf, rl.getMethod(), "POST") &&
		viewEq(buf, rl.getRequestURI().getPath(), "/a") &&
		viewEq(buf, rl.getVersion(), "HTTP/1.0"));
}

/* ── GROUP 4: ERROR CASES ── */

static void t21_unknown_method()
{
	 char *buf = "PUT / HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("21 Unknown method PUT → error",
		!rl.isComplete() && rl.isError());
}

static void t22_method_typo()
{
	 char *buf = "GEX / HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("22 Method typo GEX → error",
		rl.isError());
}

static void t23_missing_crlf()
{
	 char *buf = "GET / HTTP/1.1\r";   /* missing \n */
	RequestLine rl(0);
	feedAll(rl, buf);
	result("23 Missing LF → not complete, no error",
		!rl.isComplete() && !rl.isError());
}

static void t24_lf_without_cr()
{
	 char *buf = "GET / HTTP/1.1\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("24 LF without CR → error",
		rl.isError());
}

static void t25_bad_version_minor()
{
	 char *buf = "GET / HTTP/1.2\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("25 HTTP/1.2 → error",
		rl.isError());
}

static void t26_bad_version_string()
{
	 char *buf = "GET / HTTP/2.0\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("26 HTTP/2.0 → error",
		rl.isError());
}

static void t27_invalid_pct_in_path()
{
	 char *buf = "GET /bad%GG HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	result("27 Invalid percent-encoding %GG → error",
		rl.isError());
}

static void t28_incomplete_pct()
{
	 char *buf = "GET /bad%2 HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	/* %2 followed by space is invalid (only 1 hex digit) */
	result("28 Incomplete percent-encoding %2<SP> → error",
		rl.isError());
}

/* ── GROUP 5: EDGE / CORNER ── */

static void t29_parse_called_after_complete()
{
	 char *buf = "GET / HTTP/1.1\r\nGET /next HTTP/1.1\r\n";
	RequestLine rl(0);
	feedAll(rl, buf);
	uint16_t off1 = rl.getOffset();
	/* call parse again — must be a no-op */
	rl.Parse(buf, (uint16_t)strlen(buf));
	result("29 Extra parse() after complete is no-op",
		rl.isComplete() && rl.getOffset() == off1);
}

static void t30_reset_and_reuse()
{
	 char *buf1 = "GET / HTTP/1.1\r\n";
	 char *buf2 = "DELETE /r HTTP/1.0\r\n";
	RequestLine rl(0);
	feedAll(rl, buf1);
	bool first_ok = rl.isComplete() && !rl.isError();

	rl.reset(0);
	feedAll(rl, buf2);
	result("30 reset() then reuse with DELETE",
		first_ok &&
		rl.isComplete() && !rl.isError() &&
		viewEq(buf2, rl.getMethod(), "DELETE") &&
		viewEq(buf2, rl.getVersion(), "HTTP/1.0"));
}

/* ================================================================
** MAIN
** ================================================================ */

int	main(void)
{
	printf("\n=== RequestLine – 30 Hard Tests ===\n\n");

	printf("[ GROUP 1 ] Valid – single-shot\n");
	t01_get_simple();
	t02_post_with_path();
	t03_delete_root();
	t04_query_string();
	t05_fragment();
	t06_pct_encoded_path();
	t07_full_uri_all_parts();

	printf("\n[ GROUP 2 ] Multiple spaces / tabs\n");
	t08_multiple_spaces_before_uri();
	t09_tab_before_uri();
	t10_mixed_spaces_tabs();
	t11_multiple_spaces_before_version();
	t12_tabs_before_version();
	t13_spaces_both_sides();

	printf("\n[ GROUP 3 ] Chunked delivery\n");
	t14_byte_by_byte_get();
	t15_chunk_split_in_spaces();
	t16_chunk_split_in_method();
	t17_chunk_split_in_version();
	t18_chunk_split_at_crlf();
	t19_chunk_split_spaces_before_version();
	t20_single_byte_chunks_with_many_spaces();

	printf("\n[ GROUP 4 ] Error cases\n");
	t21_unknown_method();
	t22_method_typo();
	t23_missing_crlf();
	t24_lf_without_cr();
	t25_bad_version_minor();
	t26_bad_version_string();
	t27_invalid_pct_in_path();
	t28_incomplete_pct();

	printf("\n[ GROUP 5 ] Edge / corner\n");
	t29_parse_called_after_complete();
	t30_reset_and_reuse();

	printf("\n===================================\n");
	printf("  PASSED: %d / %d\n", g_pass, g_pass + g_fail);
	printf("  FAILED: %d / %d\n", g_fail, g_pass + g_fail);
	printf("===================================\n\n");

	return (g_fail == 0 ? 0 : 1);
}






