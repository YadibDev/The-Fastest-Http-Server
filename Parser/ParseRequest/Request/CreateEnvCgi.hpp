#ifndef CREATE_ENV_CGI_HPP
#define CREATE_ENV_CGI_HPP

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "Header.hpp"

class CreateENVCGI {
private:
    static size_t      _calculateEnvSize(const clsHeader& header);
    static void        _addBaseEnvVars(char** envp, size_t& idx, const std::string& path, 
                                       const std::string& query, const std::string& method, 
                                       const clsHeader& header);
    static void        _addHeaderEnvVars(char** envp, size_t& idx, const clsHeader& header);
    static std::string _formatCgiKey(std::string key);
    
    static const char* _knownHeaderNames[H_KNOWN_COUNT];

public:
    static char** createEnv(const clsHeader& header, const std::string& path, 
                            const std::string& query, const std::string& methodStr);
    
    static void   clearEnv(char** envp);
};

#endif