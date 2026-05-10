#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "../ServerConfig/ConfigDirectiveParser.hpp"

class clsLocation
{
private:
	short                               _flags;
	s_parse_context                     &ctx;
	stlocation                          _locationData;
	std::string                         _root; 
	std::string                         _alias;
	std::vector<s_uri_entry>            _index;
	bool                                _autoindex;
	short                               _allow_methods;
	unsigned long long                  _client_max_body_size;
	stReturnData                        _return;
	s_uri_entry                         _upload_store;
	std::map<std::string, std::string>  _cgi_pass;
	std::map<short, stErrorPagedata>    _error_pages;
	const stErrorPagedata*				_defaultErrorPage;

	bool    ParseRoot();
	bool    ParseAlias();
	bool    ParseIndex();
	bool    ParseAutoIndex();
	bool    ParseMethods();
	bool    ParseClientMaxBodySize();
	bool    ParseReturn();
	bool    ParseUploadStore();
	bool    ParseCgiPass();
	bool    ParseErrorPage();
	bool    ParseLocationDirective();
	enBlocksDirective   getLocationDirectiveType(const std::string& key);
	
	public:
	clsLocation();
	clsLocation(s_parse_context& ctxs, const std::string &sRoot
		, const std::vector<s_uri_entry>& sIndex,  unsigned long long sClient_max_body_size, bool sAutoIndex);
		clsLocation(const clsLocation &loc);
		
		bool                                        parseLocation();
		
	void	initUri();
	const std::string							&getRoot() const;
	const std::string							&getAlias() const;
	const std::vector<s_uri_entry>              &getIndex() const;
	bool										getAutoIndex() const;
	short										getAllowMethods() const;
	unsigned long long							getClientMaxBodySize() const;
	const stReturnData							&getReturn() const;
	const s_uri_entry							&getUploadStore() const;
	const std::map<std::string, std::string>	&getCgiPass() const;
	const std::map<short, stErrorPagedata>		&getErrorPages() const;
	const stErrorPagedata						*getDefaultErrorPage() const;
	const stlocation							&getLocationData() const;
	HttpError									getError() const;
};

#endif