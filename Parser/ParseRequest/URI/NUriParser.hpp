#ifndef URIPARSER_HPP
#define URIPARSER_HPP

#include "../Request/HttpTypes.hpp"
#include "../Request/Utils.hpp"
#include <cctype>

class UriParser {
private:
	enum State { STATE_PATH, STATE_QUERY, STATE_FRAGMENT, STATE_PCT_ENCODED, STATE_ERROR };
	State _state;
	State _pctReturnState;
	uint16_t _offset;
	bool _complete;
	s_view _path;
	s_view _query;
	s_view _fragment;
	uint8_t _pctDigitsRead;

	static bool isUnreserved(char c);
	static bool isSubDelim(char c);
	static bool isPchar(char c);
	static bool isHex(char c);
	static bool isPathChar(char c);
	static bool isQueryChar(char c);
	static bool isFragmentChar(char c);

	void    finishPath(const char *buffer);
	void    finishQuery(const char *buffer);
	void    finishFragment(const char *buffer);
	void	startQuery(const char *buffer);
	void    startFragmentFromPath(const char *buffer);
	void    startFragmentFromQuery(const char *buffer);
	void	enterPctEncoded(State returnState);
	void	parsePath(const char *buffer, uint16_t size);
	void	parseQuery(const char *buffer, uint16_t size);
	void	parseFragment(const char *buffer, uint16_t size);
	void	parsePctEncoded(const char *buffer, uint16_t size);

public:
	UriParser();
	void init(uint16_t startOffset);
	void parse(const char *buffer, uint16_t size);
	bool isComplete() const;
	bool isError() const;
	uint16_t getOffset() const;
	const s_view&    getPath() const;
	const s_view&    getQuery() const;
	const s_view&    getFragment() const ;
};

#endif