#include "RequestParser.hpp"

RequestParser::RequestParser(stPollRequest &request, clsServerConfig	*ServerConfig, RequestHandler	*RequestHandler)
	:
	_request(request),
	_offset(0),
	_requestLine(),
	_header(request),
	_body(request),
	_ServerConfig(ServerConfig),
	_RequestHandler(RequestHandler),
	_state(STATE_REQUEST_LINE)
{
}

void    RequestParser::init(uint16_t offset)
{
	_state = STATE_REQUEST_LINE;
	_offset = offset;
	_requestLine.reset(offset);
	_header.init(offset);
}

bool    RequestParser::LProcessRequestHandler()
{
	ProcessRequestHandler::processRequest(_requestLine, _ServerConfig, _RequestHandler);
	if (_RequestHandler->getError().isError())
	{
		_error = _RequestHandler->getError();
		return false;
	}
	return true;
}

bool	RequestParser::ParseRequestLine(uint16_t size)
{
	_requestLine.parse(_request.request_metadata, size);
	_offset = _requestLine.getOffset();
	if (_requestLine.isError())
	{
		_error = _requestLine.getError();
		return false;
	}
	if (_requestLine.isComplete())
	{
		LProcessRequestHandler();
		_state = STATE_HEADERS;
		_header.init(_offset);
	}
	return true;
}

bool    RequestParser::ParseHeader(uint16_t size)
{
	_header.Parse(size);
	_offset = _header.getOffset();

	if (_header.isError())
	{
		_error = _header.getError();
		return false;
	}
	if (_header.isComplete())
		_state = STATE_COMPLETE;

	return true;
}

bool    RequestParser::ParseBody(uint16_t size)
{
	if (_body.getState() ==  bodySteps::SETTING_VARS || _body.getState() >= bodySteps::DONE_GOOD)
	{
		memcpy(_request.io_chunk, &_request.request_metadata[_offset], size - _offset);
		*_request.read_body_ptr = size - _offset;
	}

	_body.bodyHandler((_request.read_body_ptr));

	if (_body.getState() == bodySteps::DONE_GOOD)
		_state = STATE_COMPLETE;
	else if (_body.getState() == bodySteps::DONE_WIHTERROR)
		_state = STATE_ERROR;

	return true;
}

void    RequestParser::Parse(uint16_t size)
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

		if (_state == STATE_COMPLETE || _error.isError())
			break;
		if (_offset == oldOffset && _state == oldState)
			break;
	}
}

bool		RequestParser::isComplete() const { return (_state == STATE_COMPLETE); }
bool		RequestParser::isError() const { return (_state == STATE_ERROR); }
RequestLine	RequestParser::getRequestLine() const { return _requestLine; }
HttpError	RequestParser::getError() const { return _error; }