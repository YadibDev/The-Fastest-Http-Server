#include "Request.hpp"

clsRequest::clsRequest()
    : _state(RequestStatus::READING_LINE), _startOfHeader(true), _headerParser(_headers) {}


void clsRequest::parse(const std::string& rawData)
{
    _Buffer += rawData;
    size_t pos;

    while ((pos = _Buffer.find("\r\n")) != std::string::npos)
    {
        std::string line = _Buffer.substr(0, pos);
        _Buffer.erase(0, pos + 2);

        if (_state == RequestStatus::READING_LINE)
        {
            _startLine.parseStartLine(line);
            if (_startLine.getError().isError())
            {
                _error = _startLine.getError();
                _state = RequestStatus::COMPLETED;
            }
            _state = RequestStatus::READING_HEADERS;
        }
        else if (_state == RequestStatus::READING_HEADERS)
        {
            if (line.empty())
            {
                _state = RequestStatus::READING_BODY;
                break;
            }
            if (!_headerParser.parseSingleHeader(line, _error))
                return ;
        }
        
        if (_state == RequestStatus::READING_BODY)
            break;
    }

    if (_state == RequestStatus::READING_BODY)
    {
        // Body
        std::cout << "Body completed" << std::endl;
        _state = RequestStatus::COMPLETED;
    }
}
bool clsRequest::isCompleted() const { return _state == RequestStatus::COMPLETED; }

const HttpError& clsRequest::getError() const
{
    return _error;
}