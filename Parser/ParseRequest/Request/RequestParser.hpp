#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "RequestLine.hpp"
#include "Header.hpp"
#include "../../RequestHandler/RequestHandler.hpp"
#include "../../ParseConfigFile/ServerConfig/ServerConfig.hpp"
#include "../../RequestHandler/ProcessRequestHandler.hpp"
#include "HeaderTable.hpp"
// #include "clsBody.hpp"

class RequestParser {
private:

	stPollRequest	&_request;
	uint16_t		_offset;
	RequestLine		_requestLine;
	Header			_header;
	// clsBody			_body;
	clsServerConfig	*_ServerConfig;
	RequestHandler	*_RequestHandler;
	HttpError		_error;

	bool	LProcessRequestHandler();
	bool    HProcessRequestHandler();
	bool	ParseRequestLine(uint16_t size);
	bool	ParseHeader(uint16_t size);
	bool	ParseBody();

public:
	enum State { STATE_REQUEST_LINE, STATE_HEADERS, STATE_BODY, STATE_COMPLETE, STATE_ERROR };
	State			_state;

	RequestParser(stPollRequest &request, clsServerConfig	*ServerConfig, RequestHandler	*RequestHandler);
	void			init(uint16_t offset = 0);
	bool			Parse(uint16_t size);
	bool			isComplete() const;
	bool			isError() const;
	RequestLine		getRequestLine() const;
	HttpError		getError() const;
};

#endif

