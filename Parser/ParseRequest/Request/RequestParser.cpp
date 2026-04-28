#include "RequestParser.hpp"

RequestParser::RequestParser(stPollRequest &request, RequestHandler *requestHandler)
	: _request(request),
	  _offset(0),
	  _requestLine(),
	  _header(request),
	  _body(request),
	  _ServerConfig(NULL),
	  _RequestHandler(requestHandler),
	  _state(STATE_REQUEST_LINE)
//   _body(request)
{
}

void RequestParser::init(clsServerConfig *ServerConfig, uint16_t offset)
{
	_state = STATE_REQUEST_LINE;
	_ServerConfig = ServerConfig;
	_offset = offset;
	_requestLine.init(offset);
	_header.init(offset);
	_error.setStatus(0, "");
	_RequestHandler->reset();
}

void RequestParser::init(uint16_t offset)
{
	_state = STATE_REQUEST_LINE;
	_offset = offset;
	_requestLine.init(offset);
	_header.init(offset);
	_error.setStatus(0, "");
	_RequestHandler->reset();
	_body.Reset();
}

bool RequestParser::LProcessRequestHandler()
{
	ProcessRequestHandler::processRequest(_requestLine, _ServerConfig, _RequestHandler);
	if (_RequestHandler->getError().isError())
	{
		_error = _RequestHandler->getError();
		return false;
	}
	return true;
}

bool RequestParser::ParseRequestLine(uint16_t size)
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
		if (!LProcessRequestHandler())
			return false;
		_state = STATE_HEADERS;
		_header.init(_offset);
	}
	return true;
}

bool RequestParser::ParseHeader(uint16_t size)
{
	_header.Parse(size);
	_offset = _header.getOffset();

	if (_header.isError())
	{
		_error = _header.getError();
		return false;
	}
	if (_header.isComplete())
		_state = STATE_BODY;

	return true;
}

bool RequestParser::ParseBody(uint16_t size)
{
	if (_body.getState() == clsBody::SETTING_VARS || _body.getState() >= clsBody::DONE_GOOD)
	{
		if (_request.known_headers[HttpTables::H_CONTENT_LENGTH].Hash == -1 && _request.known_headers[HttpTables::H_TRANSFER_ENCODING].Hash == -1)
		{
			_error.setStatus(411, "Length Required");
			return false;
		}
		size++;
		memcpy(_request.io_chunk, &_request.request_metadata[_offset], size - _offset);
		*_request.read_body_ptr = size - _offset;
	}

	_body.bodyHandler(_request.read_body_ptr, _ServerConfig->getMaxBodySize());

	if (_body.getState() == clsBody::DONE_GOOD)
	{
		_state = STATE_COMPLETE;
	}
	else if (_body.getState() == clsBody::DONE_WIHTERROR)
	{
		_error = _body.getError();
		_state = STATE_ERROR;
	}

	return true;
	;
}

bool RequestParser::Parse(uint16_t size)
{
	if (size >= SIZE_BUFFER)
		return (_error.setStatus(413, "Content Too Large"), false);

	while (_offset <= size)
	{
		if (_state == STATE_REQUEST_LINE)
			ParseRequestLine(size);
		else if (_state == STATE_HEADERS)
			ParseHeader(size);
		else if (_state == STATE_BODY)
		{
			ParseBody(size);
		}
		else
			break;

		if (this->getRequestLine().getMethod() == HttpTables::M_GET && _state == STATE_BODY) // this line do a lot of work
			_state = STATE_COMPLETE;
		if (_state == STATE_BODY && _request.known_headers[HttpTables::H_CONTENT_LENGTH].Hash != -1 && _request.known_headers[HttpTables::H_TRANSFER_ENCODING].Hash == -1)
		{
			if (_request.known_headers[HttpTables::H_CONTENT_LENGTH].val.len == 1 && _request.known_headers[HttpTables::H_CONTENT_LENGTH].val.Data[0] == '0')
				_state = STATE_COMPLETE;
		}
		if (_state == STATE_COMPLETE)
			return true;
		else if (_error.isError())
			return false;
	}
	return true;
}

bool RequestParser::isComplete() const { return (_state == STATE_COMPLETE); }
bool RequestParser::isError() const { return _error.isError(); }
const RequestLine &RequestParser::getRequestLine() const { return _requestLine; }
HttpError RequestParser::getError() const { return _error; }
