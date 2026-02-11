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