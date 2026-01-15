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