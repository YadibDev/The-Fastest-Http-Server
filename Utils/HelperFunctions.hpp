#ifndef HELPERFUNCTIONS_HPP
#define HELPERFUNCTIONS_HPP

#include <string>
#include <sys/time.h>

class HelperFunctions {
public:
    static std::string  trim(const std::string& str);
    static void         skipWhitespace(const std::string& str, size_t &pos);
    static bool         isCRLF(const std::string& str);
    static unsigned long long getCurrentTimeInMs();
    

private:
    HelperFunctions() {}
};

#endif