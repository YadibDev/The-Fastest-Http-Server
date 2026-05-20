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

size_t PathResolver::percentEncoded(char *buffer, size_t bufferSize, const s_view &uri) {
    size_t svIdx = 0;
    size_t bufIdx = 0;
    while (svIdx < uri.len && bufIdx < bufferSize)
    {
        if (uri.Data[svIdx] == '%' && svIdx + 2 < uri.len)
        {
            buffer[bufIdx++] = HelperFunctions::hexToDecS_view(&uri.Data[svIdx + 1], 2);
            svIdx += 3;
        }
        else
            buffer[bufIdx++] = uri.Data[svIdx++];
    }
    if (bufIdx < bufferSize)
        buffer[bufIdx] = '\0';
    return bufIdx;
}

bool PathResolver::createPhysicalPath(const clsLocation* bestLocation, char *destBuffer, const s_view& newUri, HttpError &error) {
    char CleanUri[MAX_PATH_LEN];
    const std::string &base = bestLocation->getAlias().empty() ? bestLocation->getRoot() : bestLocation->getAlias();
    size_t currentPos = base.size();

    if (currentPos >= MAX_PATH_LEN - 1)
        return (error.setStatus(414, "URI Too Long"), false);

    memcpy(destBuffer, base.c_str(), currentPos);
    size_t uriPartLen = percentEncoded(CleanUri, MAX_PATH_LEN, newUri);
    HelperFunctions::RemoveDotSegmentsDirect(CleanUri, 
											uriPartLen);
    const char* uriPart = CleanUri;

    if (!bestLocation->getAlias().empty()) {
        size_t locSize = bestLocation->getLocationData().uri.size();
        if (uriPartLen >= locSize) {
            uriPart += locSize;
            uriPartLen -= locSize;
        }
    }

    if (uriPartLen > 0) {
        if (bestLocation->getAlias().empty() && currentPos > 0) {
            if (destBuffer[currentPos - 1] != '/' && uriPart[0] != '/') {
                if (currentPos < MAX_PATH_LEN - 1)
                    destBuffer[currentPos++] = '/';
            }
        }
        if (currentPos > 0 && destBuffer[currentPos - 1] == '/' && uriPart[0] == '/') {
            uriPart++;
            uriPartLen--;
        }
        if (currentPos + uriPartLen < MAX_PATH_LEN) {
            memcpy(destBuffer + currentPos, uriPart, uriPartLen);
            currentPos += uriPartLen;
        } else
            return (error.setStatus(414, "URI Too Long"), false);
    }
    destBuffer[currentPos] = '\0';
    return true;
}