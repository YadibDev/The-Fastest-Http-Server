#pragma once

#include <iostream>
#include <string>

class HttpError {
private:
    int         _codeStatus;
    std::string _msgError;

public:
    HttpError();
    HttpError(int code, std::string msg = "");

    int                getCodeStatus() const;
    const std::string& getMsgError() const;
    bool isError() const;
    void setStatus(int code, std::string msg = "");
};

