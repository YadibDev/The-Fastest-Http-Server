#include "RequestParser.hpp"

RequestParser::RequestParser(stPollRequest &request)
    : _request(request),
      _state(STATE_REQUEST_LINE),
      _offset(0),
      _requestLine(),
      _header(request)
{
}

void    RequestParser::init(uint16_t offset)
{
    _state = STATE_REQUEST_LINE;
    _offset = offset;
    _requestLine.reset(offset);
    _header.init(offset);
}

void    RequestParser::ParseRequestLine(uint16_t size)
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

void    RequestParser::ParseHeader(uint16_t size)
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

void    RequestParser::ParseBody(uint16_t)
{
    _state = STATE_COMPLETE;
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

        if (_state == STATE_COMPLETE || _state == STATE_ERROR)
            break;
        if (_offset == oldOffset && _state == oldState)
            break;
    }
}

bool RequestParser::isComplete() const { return (_state == STATE_COMPLETE); }
bool RequestParser::isError() const { return (_state == STATE_ERROR); }
RequestLine RequestParser::getRequestLine() const { return _requestLine; }