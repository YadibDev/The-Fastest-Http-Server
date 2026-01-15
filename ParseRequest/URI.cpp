#include "URI.hpp"

URI::URI(std::string uriString)
{
    _scheme    = URIParser::extractScheme(uriString);
    _authority = URIParser::extractAuthority(uriString, _scheme);
    _userInfo  = URIParser::extractUserInfo(_authority);
    _host      = URIParser::extractHost(_authority);
    _port      = URIParser::extractPort(_authority);
    _path      = URIParser::extractPath(uriString, _scheme, _authority);
    _query     = URIParser::extractQuery(uriString);
    _fragment  = URIParser::extractFragment(uriString);
}

const std::string& URI::getScheme()    const { return _scheme; }
const std::string& URI::getHost()      const { return _host; }
int                URI::getPort()      const { return _port; }
const std::string& URI::getPath()      const { return _path; }
const std::string& URI::getQuery()     const { return _query; }
const std::string& URI::getFragment()  const { return _fragment; }