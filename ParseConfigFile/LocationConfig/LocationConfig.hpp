#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP


#include "../ServerConfig/ConfigDirectiveParser.hpp"


class clsLocation
{
private:

	clsParse<TokenType>					_parser;
	HttpError							_ERROR;
	stlocation							_locationData;
	std::string							_root;
	std::vector<std::string>			_index;
	bool								_autoindex;
	short								_allow_methods;
	unsigned long long					_client_max_body_size;
	stReturnData						_return;
	std::string							_upload_path;
	std::map<std::string, std::string>	_cgi_pass;
	std::map<short, stErrorPagedata>	_error_pages;


	bool	ParseRoot(s_parse_context& ctx);

	bool	ParseIndex(s_parse_context& ctx);

	bool	ParseAutoIndex(s_parse_context& ctx);

	bool	ParseMethods(s_parse_context& ctx);

	bool	ParseClientMaxBodySize(s_parse_context& ctx);

	bool	ParseReturn(s_parse_context& ctx);

	bool	ParseUploadPath(s_parse_context& ctx);

	bool	ParseCgiPass(s_parse_context& ctx);

	bool	ParseErrorPage(s_parse_context& ctx);
	
	bool	ParseLocationDirective(s_parse_context& ctx);
	
	enBlocksDirective	getLocationDirectiveType(const std::string& key);

public:

	clsLocation(clsParse<TokenType> parser);

	bool								parseLocation();
	
	std::string							getRoot() const;
	std::vector<std::string>			getIndex() const;
	bool								getAutoIndex() const;
	short								getAllowMethods() const;
	unsigned long long					getClientMaxBodySize() const;
	stReturnData						getReturn() const;
	std::string							getUploadPath() const;
	std::map<std::string, std::string>	getCgiPass() const;
	std::map<short, stErrorPagedata>	getErrorPages() const;

	stlocation							getLocationData() const;
	HttpError							getError() const;
};

#endif
