
#ifndef HTTP_ERROR_HPP
#define HTTP_ERROR_HPP

#include <iostream>
#include <string>

class HttpError {
private:
	int         _codeStatus;
	std::string _msgError;

public:
	HttpError();
	HttpError(int code, std::string msg = "");

	void	reset();
	int		getCodeStatus() const;
	const	std::string& getMsgError() const;
	bool	isError() const;
	void	setStatus(int code, std::string msg = "");
};

#endif