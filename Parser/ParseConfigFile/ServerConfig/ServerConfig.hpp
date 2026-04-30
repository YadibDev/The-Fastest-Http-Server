#ifndef SERVERCONFIGDATA_HPP
#define SERVERCONFIGDATA_HPP

#include <string>
#include <vector>
#include <map>
#include "../LocationConfig/LocationConfig.hpp"

class clsServerConfig {

private:
    enum e_directive {
        ROOT            = 1 << 1,
        INDEX           = 1 << 2,
        MAX_BODY_SIZE   = 1 << 3,
    };

    short                               _flags;
    std::vector<sockaddr_in>            _listens;
    std::string                         _root;           // Changed
    std::vector<s_uri_entry>            _index;          // Changed
    std::map<short, stErrorPagedata>    _error_pages;
    bool                                _autoindex;
    stReturnData                        _return;
    size_t                              _max_body_size;
    std::vector<clsLocation>            _LocationExact;
    std::vector<clsLocation>            _LocationPrefix;
    s_parse_context                     &ctx;
    
    bool                ParseListen();
    bool                ParseErrorPage();
    bool                ParseClientMaxBodySize();
    bool                ParseLocation();
    bool                ParseRoot();
    bool                ParseReturn();
    bool                ParseIndex();
    bool                ParseAutoIndex();
    enBlocksDirective   getServerDirectiveType(const std::string& key);
    bool                ParseServerDirective();

public:
    clsServerConfig(s_parse_context &ctx);
    ~clsServerConfig();

    bool    parseBlockServer();

    std::vector<sockaddr_in>            getListens() const;
    const std::map<short, stErrorPagedata>    &getErrorPages() const;
    size_t                              getMaxBodySize() const;
    const std::vector<clsLocation> &    getLocationExact() const;
    const std::vector<clsLocation> &    getLocationPrefix() const;
    const std::string                   &getRoot() const;    // Changed
    const std::vector<s_uri_entry>      &getIndex() const;   // Changed
    const stReturnData                  &getReturn() const;
    bool                                getAutoIndex() const;
    void                                initUri();

    HttpError getError() const;
};

#endif