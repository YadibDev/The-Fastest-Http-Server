#include "UriParser.hpp"

UriParser::URIParser()
{
    
}

void    UriParser::init(uint16_t startOffset)
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

bool UriParser::isUnreserved(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);
    return (std::isalnum(uc) || c == '-' || c == '.' || c == '_' || c == '~');
}

bool UriParser::isSubDelim(char c)
{
    return (
        c == '!' || c == '$' || c == '&' || c == '\'' ||
        c == '(' || c == ')' || c == '*' || c == '+' ||
        c == ',' || c == ';' || c == '='
    );
}

bool UriParser::isPchar(char c)
{
    return (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@');
}

bool UriParser::isHex(char c)
{
    return (
        (c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F')
    );
}

bool UriParser::isPathChar(char c)
{
    return (isPchar(c) || c == '/');
}

bool UriParser::isQueryChar(char c)
{
    return (isPchar(c) || c == '/' || c == '?');
}

bool UriParser::isFragmentChar(char c)
{
    return (isPchar(c) || c == '/' || c == '?');
}

void    UriParser::finishPath()
{
    _path.len = _offset - _path.Offsets;
}

void    UriParser::finishQuery()
{
    _query.len = _offset - _query.Offsets;
}

void    UriParser::finishFragment()
{
    _fragment.len = _offset - _fragment.Offsets;
}

void    UriParser::startQuery()
{
    finishPath();
    _offset++;
    _query.Offsets = _offset;
    _query.len = 0;
    _state = STATE_QUERY;
}

void    UriParser::startFragmentFromPath()
{
    finishPath();
    _offset++;
    _fragment.Offsets = _offset;
    _fragment.len = 0;
    _state = STATE_FRAGMENT;
}

void    UriParser::startFragmentFromQuery()
{
    finishQuery();
    _offset++;
    _fragment.Offsets = _offset;
    _fragment.len = 0;
    _state = STATE_FRAGMENT;
}

void    UriParser::enterPctEncoded(State returnState)
{
    _pctReturnState = returnState;
    _pctDigitsRead = 0;
    _offset++;
    _state = STATE_PCT_ENCODED;
}

void    UriParser::parsePath(const char *buffer, uint16_t size)
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

void    UriParser::parseQuery(const char *buffer, uint16_t size)
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

void    UriParser::parseFragment(const char *buffer, uint16_t size)
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

void    UriParser::parsePctEncoded(const char *buffer, uint16_t size)
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

void    UriParser::parse(const char *buffer, uint16_t size)
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

bool        UriParser::isComplete() const { return _complete; }
bool        UriParser::isError() const { return (_state == STATE_ERROR); }
uint16_t    UriParser::getOffset() const { return _offset; }
s_view      UriParser::getPath() const { return _path; }
s_view      UriParser::getQuery() const { return _query; }
s_view      UriParser::getFragment() const { return _fragment; }