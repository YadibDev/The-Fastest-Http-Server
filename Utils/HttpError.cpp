#include "HttpError.hpp"

HttpError::HttpError() : _codeStatus(0), _msgError("")
{
    
}

HttpError::HttpError(int code, std::string msg) : _codeStatus(code), _msgError(msg) {}

int HttpError::getCodeStatus() const { 
    return _codeStatus;
}

const std::string& HttpError::getMsgError() const { 
    return _msgError; 
}

bool HttpError::isError() const { 
    return (_codeStatus);
}

void HttpError::setStatus(int code, std::string msg) {
    _codeStatus = code;
    _msgError = msg;
}