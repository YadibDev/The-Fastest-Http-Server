#ifndef URIPARSER_HPP
#define URIPARSER_HPP

#include "../Request/HttpTypes.hpp"
#include "../Request/Utils.hpp"
#include "../../../Utils/HttpError.hpp"
#include <cctype>

class UriParser {
public:
	enum State {
		STATE_SCHEMA,
		STATE_AUTHORITY,
		STATE_HOST,
		STATE_PORT,
		STATE_PATH,
		STATE_QUERY,
		STATE_FRAGMENT,
		STATE_PCT_ENCODED,
		STATE_ERROR,
		UNKNOW
	};

	UriParser();
	void init(uint16_t startOffset);
	void parse(const char* buffer, uint16_t size);

	bool                isComplete() const;
	bool                isError() const;
	uint16_t            getOffset() const;
	unsigned short      getPort() const;
	const s_view&       getAuthority() const;
	const s_view&       getHost() const;
	const s_view&       getPath() const;
	const s_view&       getQuery() const;
	const s_view&       getFragment() const;
	const HttpError&    getError() const;
	bool				AuthorityExist();

private:
	State			_state;
	State			_pctReturnState;
	uint16_t		_offset;
	bool			_complete;
	uint8_t			_pctDigitsRead;
	uint8_t			_shemaIndex;
	bool			_AuthorityExist;
	unsigned short	_port;

	s_view			_authority;
	s_view			_host;
	s_view			_path;
	s_view			_query;
	s_view			_fragment;

	HttpError		_error;

	bool isUnreserved(char c);
	bool isSubDelim(char c);
	bool isPchar(char c);
	bool isHex(char c);
	bool isPathChar(char c);
	bool isQueryChar(char c);
	bool isFragmentChar(char c);

	bool validateIPv4(const char* data, size_t len);
	bool validateRegName(const char* data, size_t len);

	void finishAuthority(const char* buffer);
	void finishPath(const char* buffer);
	void finishQuery(const char* buffer);
	void finishFragment(const char* buffer);
	void enterPctEncoded(State returnState);

	void parseScheme(const char* buffer, uint16_t size);
	void parseAuthority(const char* buffer, uint16_t size);
	void _parseHost(const char* buffer, uint16_t size);
	void _parsePort(const char* buffer, uint16_t size);
	void parsePath(const char* buffer, uint16_t size);
	void parseQuery(const char* buffer, uint16_t size);
	void parseFragment(const char* buffer, uint16_t size);
	void parsePctEncoded(const char* buffer, uint16_t size);
};

#endif