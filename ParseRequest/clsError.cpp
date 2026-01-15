#include <string>
#include <cctype>
#include <iostream>

class ValidationResult {
	private:
	short		statuCode;
	std::string	errorMessage;


	static ValidationResult Success() { return {200, ""}; }
	static ValidationResult Failure(std::string msg) { return {false, msg}; }
};