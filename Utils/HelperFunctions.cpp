#include "HelperFunctions.hpp"

std::string HelperFunctions::trim(const std::string& str) {
	const std::string whitespace = " \t";
	
	size_t first = str.find_first_not_of(whitespace);
	
	if (first == std::string::npos) {
		return "";
	}
	
	size_t last = str.find_last_not_of(whitespace);
	
	return str.substr(first, (last - first + 1));
}

void HelperFunctions::skipWhitespace(const std::string& str, size_t &pos) {
	while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\t'))
		pos++;
}

bool HelperFunctions::isCRLF(const std::string& str) {
	return (str == "\r\n");
}

int hexCharToDec(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return -1;
}

long HelperFunctions::hexToDec(const std::string& hex)
{
	long decimalValue = 0;
	
	for (std::size_t i = 0; i < hex.length(); ++i)
	{
		int digit = hexCharToDec(hex[i]);
		
		if (digit == -1)
			return -1;
		
		decimalValue = (decimalValue << 4) | digit;
	}
	
	return decimalValue;
}

bool HelperFunctions::is_numeric(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++)
		if (!std::isdigit(str[i])) return false;
	return true;
}


bool HelperFunctions::is_CTLsString(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++)
		if (std::iscntrl(str[i])) return true;
	return false;
}

bool    HelperFunctions::checkIfTheFirstWord(std::string str, std::string Start, size_t POS)
{

	for (size_t i = POS; i < str.size(); i++)
	{
		if (str[i] == ' ' || str[i] == '\t')
			break ;
		if (str[i] != Start[i])
			return (false);
	}
	return (true);
}

bool HelperFunctions::isLWS(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::string HelperFunctions::normalizeLWS(const std::string& input)
{
    std::string result;
    size_t i = 0;
    bool inSpace = false;

    while (i < input.size() && isLWS(input[i]))
        i++;

    for (; i < input.size(); i++)
    {
        if (isLWS(input[i]))
            inSpace = true;
        else
        {
            if (inSpace && !result.empty())
                result += ' ';
            result += input[i];
            inSpace = false;
        }
    }
    return result;
}

bool HelperFunctions::myIsspace(std::string str, size_t pos)
{
	for (size_t i = pos; i < str.size(); i++)
	{
		if (isLWS(str[i]))
			return (true);
	}
	return (false);
}

std::vector<std::string> HelperFunctions::splitCommaSeparated(const std::string& value)
{
    std::vector<std::string> result;
    std::string current;

    for (size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == ',')
        {
            current = normalizeLWS(current);
            if (!current.empty())
                result.push_back(current);
            current.clear();
        }
        else
            current += value[i];
    }

    current = normalizeLWS(current);
    if (!current.empty())
        result.push_back(current);

    return result;
}
