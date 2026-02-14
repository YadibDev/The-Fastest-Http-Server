#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>
#include <string>

class HttpError
{
private:
    int _codeStatus;
    std::string _msgError;

public:
    HttpError()
    {
        _codeStatus = 0;
        _msgError = "";
    }
    HttpError(int status, std::string str = "")
    {
        _codeStatus = status;
        _msgError = str;
    }

    int getCodeStatus() const { return _codeStatus; }
    const std::string &getMsgError() const { return _msgError; }

    bool isError() const { return _codeStatus; };
    void setStatus(int code, std::string msg = "")
    {
        _codeStatus = code;
        _msgError = msg;
    }
};

#endif