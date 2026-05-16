#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include "../URI/NUriParser.hpp"

class RequestLine {
private:
	enum State { STATE_START, STATE_METHOD, STATE_URI, STATE_VERSION, STATE_CR, STATE_LF, STATE_COMPLETE, STATE_ERROR };

	State				_state;
	HttpTables::eMethod	_methodType;
	uint16_t			_offset;
	bool				_uriReady;
	bool				_versionReady;
	bool				_versionMinorRead;
	s_view				_method;
	s_view				_version;
	uint8_t				_methodIndex;
	uint8_t				_versionIndex;
	UriParser			_uriParser;
	HttpError			_error;

	const char			*methodName() const;
	bool				selectMethod(const char *buffer, uint16_t size);
	bool				parseMethod(const char *buffer, uint16_t size);
	bool				parseUri(const char *buffer, uint16_t size);
	bool				parseVersion(const char *buffer, uint16_t size);
	bool				parseCR(const char *buffer, uint16_t size);
	bool				parseLF(const char *buffer, uint16_t size);

public:
	RequestLine();
	void				init(uint16_t startOffset);
	bool				parse(const char *buffer, uint16_t size);
	bool				isComplete() const;
	bool				isError() const;
	HttpTables::eMethod	getMethod() const;
	s_view				getVersion() const;
	const UriParser		&getRequestURI() const;
	UriParser		&getRequestURI(); // add by yadib
	uint16_t			getOffset() const;
	HttpError			getError() const;
};

#endif