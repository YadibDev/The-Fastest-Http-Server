#include "PathResolver.hpp"

bool PathResolver::viewEqualsString(const s_view &view, const std::string &str) {
    if (view.len != str.size()) return false;
    return (std::memcmp(view.Data, str.data(), view.len) == 0);
}

bool PathResolver::viewStartsWithString(const s_view &view, const std::string &prefix) {
    if (view.len < prefix.size()) return false;
    return (std::memcmp(view.Data, prefix.data(), prefix.size()) == 0);
}

sPathType::e_path_type PathResolver::checkPath(char *path, UriStatus &flags, size_t &size) {
    if (!path) return sPathType::PATH_NOT_FOUND;
    struct stat st;
    if (stat(path, &st) != 0) return sPathType::PATH_NOT_FOUND;

    size = st.st_size;
    flags.can_read = st.st_mode & (S_IRUSR | S_IRGRP | S_IROTH);
    flags.can_write = st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH);
    flags.can_execute = st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH);

    if (S_ISREG(st.st_mode)) {
        flags.is_file = true;
        return sPathType::PATH_FILE;
    } else if (S_ISDIR(st.st_mode)) {
        flags.is_dir = true;
        return sPathType::PATH_DIR;
    }
    return (sPathType::PATH_OTHER);
}

size_t PathResolver::percentEncoded(s_view &uri) {
    size_t r = 0;
    size_t w = 0;

    while (r < uri.len)
    {
        if (uri.Data[r] == '%' && r + 2 < uri.len)
        {
            uri.Data[w++] = HelperFunctions::hexToDecS_view(&uri.Data[r + 1], 2);
            r += 3;
        }
        else
            uri.Data[w++] = uri.Data[r++];
    }
    uri.len = w;
    return w;
}

bool PathResolver::createPhysicalPath(const clsLocation* bestLocation, char *destBuffer, const s_view& newUri, HttpError &error) {
    const std::string &base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
    size_t currentPos = base.size();

    if (currentPos >= MAX_PATH_LEN - 1)
        return (error.setStatus(414, "URI Too Long"), false);

    memcpy(destBuffer, base.c_str(), currentPos);
    size_t uriPartLen = newUri.len ;

    const char* uriPart = newUri.Data;
    short uriIndex = 0;

    if (!bestLocation->getAlias().empty()) {
        size_t locSize = bestLocation->getLocationData().uri.size();
        if (uriPartLen >= locSize) {
            uriIndex += locSize;
            uriPartLen -= locSize;
        }
    }

    if (uriPartLen > 0) {
        if (bestLocation->getAlias().empty() && currentPos > 0) {
            if (destBuffer[currentPos - 1] != '/' && uriPart[uriIndex] != '/') {
                if (currentPos < MAX_PATH_LEN - 1)
                    destBuffer[currentPos++] = '/';
            }
        }
        if (currentPos > 0 && destBuffer[currentPos - 1] == '/' && uriPart[uriIndex] == '/') {
            uriIndex++;
            uriPartLen--;
        }
        if (currentPos + uriPartLen < MAX_PATH_LEN) {
            memcpy(destBuffer + currentPos, &uriPart[uriIndex], uriPartLen);
            currentPos += uriPartLen;
        } else
            return (error.setStatus(414, "URI Too Long"), false);
    }
    destBuffer[currentPos] = '\0';
    return true;
}