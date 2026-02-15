#ifndef HELPERFUNCTIONS_HPP
#define HELPERFUNCTIONS_HPP

#include <string>
#include "HelperString.hpp"

class HelperFunctions {
public:
	static std::string	trim(const std::string& str);
	static void			skipWhitespace(const std::string& str, size_t &pos);
	static bool			isCRLF(const std::string& str);
	static long			hexToDec(const std::string& hex);
	static bool			is_numeric(const std::string& str);
	static bool			is_CTLsString(const std::string& str);
	static bool			checkIfTheFirstWord(std::string str, std::string Start, int POS);
	static std::string	normalizeLWS(const std::string& input);
	static bool			isLWS(char c);

private:
	HelperFunctions() {}
};

#endif