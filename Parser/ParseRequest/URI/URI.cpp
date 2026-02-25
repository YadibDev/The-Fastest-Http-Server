#include "URI.hpp"

URI::URI(std::string uriString)
{
    URIParser::extractScheme(uriString, _scheme);
    URIParser::extractAuthority(uriString, _scheme, _authority);
    URIParser::extractUserInfo(_authority, _userInfo);
    URIParser::extractHost(_authority, _host);
    URIParser::extractPort(_authority, _port);
    URIParser::extractPath(uriString, _authority, _path);
    URIParser::extractQuery(uriString, _query);
    URIParser::extractFragment(uriString, _fragment);
}

const std::string& URI::getScheme()    const { return _scheme; }
const std::string& URI::getHost()      const { return _host; }
int                URI::getPort()      const { return _port; }
const std::string& URI::getPath()      const { return _path; }
const std::string& URI::getQuery()     const { return _query; }
const std::string& URI::getFragment()  const { return _fragment;}