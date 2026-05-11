#ifndef PATH_RESOLVER_HPP
#define PATH_RESOLVER_HPP

#include <string>
#include <vector>
#include <cstring>
#include <sys/stat.h>
#include "../ParseRequest/Request/HttpTypes.hpp"
#include "../../Utils/HelperFunctions.hpp"
#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp" 

#ifndef MAX_PATH_LEN
# define MAX_PATH_LEN 4096
#endif

class PathResolver {
public:
    static bool viewEqualsString(const s_view &view, const std::string &str);
    static bool viewStartsWithString(const s_view &view, const std::string &prefix);
    static sPathType::e_path_type checkPath(char *path, UriStatus &flags, size_t &size);
    static size_t percentEncoded(char *buffer, size_t bufferSize, const s_view &uri);
    static bool createPhysicalPath(const clsLocation* bestLocation, char *destBuffer, const s_view& newUri, HttpError &error);
};

#endif