#include "CreateEnvCgi.hpp"
#include <cstring>

const char* CreateENVCGI::_knownHeaderNames[header_id::H_KNOWN_COUNT] = {
	"HTTP_HOST", "content-length", "content-type", "transfer-encoding", 
	"HTTP_CONNECTION", "HTTP_EXPECT", "HTTP_USER_AGENT", "COOKIUE cookie"
};

char** CreateENVCGI::createEnv(const clsHeader& header, const std::string& path, 
							   const std::string& query, const std::string& methodStr) {
	size_t totalSize = _calculateEnvSize(header);
	char** envp = new(std::nothrow) char*[totalSize + 1];
	if (!envp) return NULL;

	size_t idx = 0;
	_addBaseEnvVars(envp, idx, path, query, methodStr, header);
	_addHeaderEnvVars(envp, idx, header);
	envp[idx] = NULL;

	for (size_t i = 0; i < idx; ++i)
	{
		if (!envp[i])
		{
			clearEnv(envp);
			return NULL;
		}
	}
	return envp;
}

bool isBlackList(header_id::e_header_id headerId)
{
    return (headerId == header_id::H_CONTENT_TYPE || 
            headerId == header_id::H_CONTENT_LENGTH ||
            headerId == header_id::H_TRANSFER_ENCODING ||
            headerId == header_id::H_CONNECTION ||
            headerId == header_id::H_EXPECT);
}


size_t CreateENVCGI::_calculateEnvSize(const clsHeader& header) {
	size_t knownCount = 0;
	uint32_t mask = header.getHeaderMask();
	for (int i = 0; i < header_id::H_KNOWN_COUNT; ++i)
	{
		if ((mask & (1 << i)) && i != header_id::H_CONTENT_TYPE && i != header_id::H_CONTENT_LENGTH)
			knownCount++;
	}
	return 7 + knownCount + header.getUnknownHeaders().size();
}

void CreateENVCGI::_addBaseEnvVars(char** envp, size_t& idx, const std::string& path, 
								   const std::string& query, const std::string& method, 
								   const clsHeader& header) {
	envp[idx++] = strdup(("GATEWAY_INTERFACE=CGI/1.1"));
	envp[idx++] = strdup(("SERVER_PROTOCOL=HTTP/1.1"));
	envp[idx++] = strdup(("REQUEST_METHOD=" + method).c_str());
	envp[idx++] = strdup(("PATH_INFO=" + path).c_str());
	envp[idx++] = strdup(("QUERY_STRING=" + query).c_str());
	envp[idx++] = strdup(("CONTENT_TYPE=" + header.getHeader(header_id::H_CONTENT_TYPE)).c_str());
	envp[idx++] = strdup(("CONTENT_LENGTH=" + header.getHeader(header_id::H_CONTENT_LENGTH)).c_str());
}

void CreateENVCGI::_addHeaderEnvVars(char** envp, size_t& idx, const clsHeader& header) {
	uint32_t mask = header.getHeaderMask();
	for (int i = 0; i < header_id::H_KNOWN_COUNT; ++i) {
		if ((mask & (1 << i)) && !isBlackList(static_cast<header_id::e_header_id>(i)))
		{
			std::string key = "HTTP_" + _formatCgiKey(_knownHeaderNames[i]);
			envp[idx++] = strdup((key + "=" + header.getHeader(static_cast<header_id::e_header_id>(i))).c_str());
		}
	}
	const std::map<std::string, std::string>& unknown = header.getUnknownHeaders();
	for (std::map<std::string, std::string>::const_iterator it = unknown.begin(); it != unknown.end(); ++it)
		envp[idx++] = strdup(("HTTP_" + _formatCgiKey(it->first) + "=" + it->second).c_str());
}

std::string CreateENVCGI::_formatCgiKey(std::string key) {
	for (size_t i = 0; i < key.length(); ++i)
	{
		key[i] = std::toupper(key[i]);
		if (key[i] == '-') key[i] = '_';
	}
	return key;
}

void CreateENVCGI::clearEnv(char** envp)
{
	if (!envp) return;
	for (size_t i = 0; envp[i]; ++i) free(envp[i]);
	delete[] envp;
}