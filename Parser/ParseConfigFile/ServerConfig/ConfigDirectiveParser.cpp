#include "ConfigDirectiveParser.hpp"


bool ConfigDirectiveParser::validateDirectoryPath(const std::string& path,
                                                  s_parse_context& ctx,
                                                  const std::string& directiveName)
{
    struct stat st;

    if (stat(path.c_str(), &st) != 0) {
        ctx.error.setStatus(400, directiveName + " path does not exist: " + path);
        return false;
    }

    if (!S_ISDIR(st.st_mode)) {
        ctx.error.setStatus(400, directiveName + " path is not a directory: " + path);
        return false;
    }

    if (access(path.c_str(), R_OK | X_OK) != 0) {
        ctx.error.setStatus(400,
            "Insufficient permissions on " + directiveName + " directory: " + path);
        return false;
    }

    return true;
}

bool ConfigDirectiveParser::parseLocationPath(s_parse_context& ctx, stlocation& loc) {
    
    if (ctx.parser.peek().type != TOKEN_WORD) {
        return (ctx.error.setStatus(400, "Location Error: Expected a modifier or a URI path"), false);
    }

    std::string token = ctx.parser.peek().value;

    if (token == "=")
    {
        loc.matchType = stlocation::EXACT;
        ctx.parser.advance();
        skipWhitespace(ctx.parser);
    }
    else if (token == "^~")
    {
        loc.matchType = stlocation::PREFIX;
        ctx.parser.advance();
        skipWhitespace(ctx.parser);
    }
    else if (token == "~" || token == "~*")
        return (ctx.error.setStatus(400, "Location Error: Regex modifiers (~, ~*) are not supported"), false);
    else
        loc.matchType = stlocation::PREFIX;

    if (ctx.parser.peek().type != TOKEN_WORD)
        return (ctx.error.setStatus(400, "Location Error: Missing URI path after modifier"), false);

    loc.uri = ctx.parser.peek().value;

    if (loc.uri.empty())
        return (ctx.error.setStatus(400, "Location Error: URI path cannot be empty"), false);

    if (loc.uri[0] != '/')
        return (ctx.error.setStatus(400, "Location Error: URI path must start with '/' (found: '" + loc.uri + "')"), false);

    ctx.parser.advance();
    skipWhitespace(ctx.parser);

    if (ctx.parser.peek().type != TOKEN_LBRACE)
        return (ctx.error.setStatus(400, "Location Error: Expected '{' after URI path"), false);

    ctx.parser.advance();
    skipWhitespace(ctx.parser);

    return true;
}

std::string ConfigDirectiveParser::ParseRoot(s_parse_context& ctx) {
    ctx.parser.advance(); 
    if (ctx.parser.peek().type != TOKEN_WORD)
        return (ctx.error.setStatus(400, "Expected path after 'root'"), "");

    std::string root;
    ctx.error = URIParser::normalizePath(ctx.parser.peek().value, root);
    if (ctx.error.isError()) return "";

    if (!validateDirectoryPath(root, ctx, "root"))
        return "";

    ctx.parser.advance();
    if (ctx.parser.peek().type != TOKEN_SEMICOLON)
        return (ctx.error.setStatus(400, "Missing ';' after root"), "");

    ctx.parser.advance();
    skipWhitespace(ctx.parser);

    return root;
}

std::string ConfigDirectiveParser::parseAlias(s_parse_context& ctx)
{
    ctx.parser.advance();
    if (ctx.parser.peek().type != TOKEN_WORD)
        return (ctx.error.setStatus(400, "Expected path after 'alias'"), "");

    std::string alias;
    ctx.error = URIParser::normalizePath(ctx.parser.peek().value, alias);
    if (ctx.error.isError()) return "";

    if (!validateDirectoryPath(alias, ctx, "alias"))
        return ""; 

    ctx.parser.advance();
    if (ctx.parser.peek().type != TOKEN_SEMICOLON)
        return (ctx.error.setStatus(400, "Missing ';' after alias"), "");

    ctx.parser.advance();
    skipWhitespace(ctx.parser);

    return alias;
}

unsigned long long ConfigDirectiveParser::ParseClientMaxBodySize(s_parse_context& ctx) {
	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (ctx.error.setStatus(400, "Expected size value"), 0);

	short numLength = 0;
	long long num = extractNumericPart(ctx.parser.peek().value, numLength);
	std::string unitStr = ctx.parser.peek().value.substr(numLength);
	char unit = (unitStr.empty()) ? ' ' : unitStr[0];

	unsigned long long bytes = convertToBytes(num, unit, ctx.error);
	ctx.parser.advance();

	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Missing ';' after max_body_size"), 0);

	ctx.parser.advance();
	skipWhitespace(ctx.parser);

	return bytes;
}

std::vector<std::string> ConfigDirectiveParser::ParseIndex(s_parse_context& ctx) {
	std::vector<std::string> indixes;

	ctx.parser.advance();

	while (ctx.parser.peek().type == TOKEN_WORD)
	{
        std::string index = ctx.parser.peek().value;
        if (index.empty())
            return (ctx.error.setStatus(400, "Index Is Empty"), indixes);
		indixes.push_back(index);
		ctx.parser.advance();
		
	}

	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Missing ';' after index list"), indixes);

	ctx.parser.advance();
	skipWhitespace(ctx.parser);
	return indixes;
}

bool ConfigDirectiveParser::ParseAutoIndex(s_parse_context& ctx) {
	ctx.parser.advance();
	bool result = false;
	std::string val = ctx.parser.peek().value;

	if (val == "on") result = true;
	else if (val == "off") result = false;
	else ctx.error.setStatus(400, "invalid value \"" + val + "\" in \"autoindex\" directive, it must be \"on\" or \"off");

	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Missing ';' after autoindex"), false);
	
	ctx.parser.advance();
	skipWhitespace(ctx.parser);

	return result;
}

sockaddr_in ConfigDirectiveParser::ParseListen(s_parse_context& ctx)
{
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(sockaddr_in));
	ctx.parser.advance();

	if (ctx.parser.peek().type == TOKEN_WORD) {
		addr = setSockaddr_in(ctx.parser.peek().value, ctx.error);
	} else {
		ctx.error.setStatus(400, "Syntax Error: Expected address/port after 'listen'");
		return addr;
	}

	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON) {
		ctx.error.setStatus(400, "Syntax Error: Missing ';' after listen");
		return addr;
	}

	ctx.parser.advance();
	skipWhitespace(ctx.parser);
	return addr;
}

stReturnData ConfigDirectiveParser::ParseReturn(s_parse_context& ctx) {
	stReturnData returnData;
	char* end;

	ctx.parser.advance();

	const Token<TokenType>& first = ctx.parser.peek();
	long code = std::strtol(first.value.c_str(), &end, 10);
	bool isNumeric = (*end == '\0');

	if (!isNumeric) {
		if (first.value.compare(0, 7, "http://") != 0) {
			ctx.error.setStatus(400, "invalid return value \"" + first.value + "\"");
			return returnData;
		}
		returnData.code = 302;
		returnData.value.raw_path = first.value;
		ctx.parser.advance();
	}
	else {
		returnData.code = (int)code;
		ctx.parser.advance();

		if (ctx.parser.peek().type == TOKEN_WORD) {
			returnData.value.raw_path = ctx.parser.peek().value;
			ctx.parser.advance();
		}
	}

	if (ctx.parser.peek().type != TOKEN_SEMICOLON) {
		ctx.error.setStatus(400, "Syntax Error: Missing ';' after return");
		return returnData;
	}
	ctx.parser.advance();
	skipWhitespace(ctx.parser);

	return returnData;
}

std::string ConfigDirectiveParser::ParseUploadStore(s_parse_context& ctx) {
	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (ctx.error.setStatus(400, "Expected path after 'upload_path'"), "");

	std::string path = ctx.parser.peek().value;
	if (HelperFunctions::isValidPath(path, true) != 200)
		return (ctx.error.setStatus(400, "Invalid upload path -> " + path), "");
	
	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		ctx.error.setStatus(400, "Missing ';' after upload_path");

	ctx.parser.advance();
	skipWhitespace(ctx.parser);
	return path;
}

std::string ConfigDirectiveParser::ParseUploadLocation(s_parse_context& ctx)
{
	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (ctx.error.setStatus(400, "Expected path after 'upload_location'"), "");

	std::string path = ctx.parser.peek().value;
	
	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		ctx.error.setStatus(400, "Missing ';' after upload_path");

	ctx.parser.advance();
	skipWhitespace(ctx.parser);
	return path;
}

void ConfigDirectiveParser::ParseCGI(s_parse_context& ctx, std::map<std::string, std::string>& cgiMap) {
	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_WORD) {
		ctx.error.setStatus(400, "CGI Error: Expected extension");
		return;
	}
	const std::string extension = ctx.parser.peek().value;
	ctx.parser.advance();

	if (ctx.parser.peek().type != TOKEN_WORD) {
		ctx.error.setStatus(400, "CGI Error: Expected binary path");
		return;
	}
	const std::string binPath = ctx.parser.peek().value;

	if (HelperFunctions::isValidPath(binPath, false) != 200)
	{
		ctx.error.setStatus(400, "No such file 'Executable Path'");
		return;
	}

	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON) {
		ctx.error.setStatus(400, "CGI Error: Expected ';'");
		return;
	}
	ctx.parser.advance();
	skipWhitespace(ctx.parser);
	cgiMap[extension] = binPath;
}

short ConfigDirectiveParser::parseMethods(s_parse_context& ctx) {
	short combinedMethods = 0;
	ctx.parser.advance(); 

	while (ctx.parser.peek().type == TOKEN_WORD) {
		std::string method = ctx.parser.peek().value;
		if (method == "GET") combinedMethods |= 1;
		else if (method == "POST") combinedMethods |= 2;
		else if (method == "DELETE") combinedMethods |= 4;
		else return (ctx.error.setStatus(400, "Unknown method: " + method), 0);
		ctx.parser.advance();
	}

	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Expected ';' after methods"), 0);

	ctx.parser.advance();
	skipWhitespace(ctx.parser);
	return combinedMethods;
}

bool	isValidStatusCode(short code)
{
	return code >= 100 && code <= 599;
}

bool ConfigDirectiveParser::ParseErrorPage(s_parse_context& ctx, std::map<short, stErrorPagedata> &error_pages) {
	std::vector<short> codes;
	short responseOverride = -1;
	std::string uri = "";

	ctx.parser.advance();

	while (ctx.parser.peek().type == TOKEN_WORD && HelperFunctions::is_numeric(ctx.parser.peek().value))
	{
		short code = (short)std::atoi(ctx.parser.peek().value.c_str());
		if (!isValidStatusCode(code))
			return (ctx.error.setStatus(400, "Invalid status code in error_page"), false);
		codes.push_back(code);
		ctx.parser.advance();
	}

	if (ctx.parser.peek().type == TOKEN_WORD && ctx.parser.peek().value[0] == '=')
	{
		std::string &resStr = ctx.parser.peek().value;
		if (resStr.size() > 1)
			responseOverride = (short)std::atoi(resStr.substr(1).c_str());
		ctx.parser.advance();
	}

	if (ctx.parser.peek().type == TOKEN_WORD)
	{
		uri = ctx.parser.peek().value;
		ctx.parser.advance();
	}
	else
		return (ctx.error.setStatus(400, "Syntax Error: Missing URI in error_page"), false);

	stErrorPagedata data;
	data.uri.raw_path = uri;

	if (uri[0] != '/')
	{
		if (responseOverride != -1)
		{
			if (responseOverride != 301 && responseOverride != 302 && 
				responseOverride != 303 && responseOverride != 307 && responseOverride != 308) {
				responseOverride = 302;
			}
		}
		else
			responseOverride = 302;
	}

	data.response = responseOverride;

	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Syntax Error: Missing ';' after error_page"), false);
		
	ctx.parser.advance();
	skipWhitespace(ctx.parser);

	for (size_t i = 0; i < codes.size(); ++i)
		error_pages[codes[i]] = data;
	return true;
}



// Private Helpers


void	ConfigDirectiveParser::DefineUri(s_uri_entry &uri)
{
	if (uri.getPath()[0] == '/')
		uri.flags.is_abs_path = true;
	else if (!uri.raw_path.compare(0, 7, "http://"))
		uri.flags.is_abs_uri = true;
	else
		uri.flags.is_relative = true;
}

unsigned long long ConfigDirectiveParser::convertToBytes(long long value, char unit, HttpError& error) {
	unsigned long long multiplier = 1;
	switch (unit) {
		case 'g': case 'G': multiplier = 1024ULL * 1024 * 1024; break;
		case 'm': case 'M': multiplier = 1024ULL * 1024; break;
		case 'k': case 'K': multiplier = 1024ULL; break;
		case ' ': multiplier = 1; break;
		default: error.setStatus(1, "\"client_max_body_size\" directive invalid value "); return 0;
	}
	return (unsigned long long)(value * multiplier);
}

long long ConfigDirectiveParser::extractNumericPart(const std::string& str, short &length) {
	long long result = 0;
	size_t i = 0;
	while (i < str.length() && std::isdigit(str[i])) {
		result = result * 10 + (str[i] - '0');
		i++;
	}
	length = i;
	return result;
}


sockaddr_in ConfigDirectiveParser::setSockaddr_in(const std::string& input, HttpError& error) {

	sockaddr_in serv_addr;
	if (input.empty())
	{
		error.setStatus(400, "setSockaddr_in: empty input");
		return serv_addr;
	}

	size_t colon_pos = input.rfind(':');
	if (colon_pos == std::string::npos || colon_pos == 0 || colon_pos == input.size() - 1) {
		error.setStatus(400, "setSockaddr_in: missing or misplaced ':' in '" + input + "'");
		return serv_addr;
	}

	std::string ip_part = input.substr(0, colon_pos);
	std::string port_part = input.substr(colon_pos + 1);

	// if (ip_part == "*" || ip_part == "localhost")
		// ip_part = "0.0.0.0";

	struct in_addr ipv4_addr;
	if (inet_pton(AF_INET, ip_part.c_str(), &ipv4_addr) != 1) {
		error.setStatus(400, "setSockaddr_in: invalid IPv4 address: '" + ip_part + "'");
		return serv_addr;
	}

	char *endptr;
	long port = std::strtol(port_part.c_str(), &endptr, 10);
	if (*endptr != '\0' || port < 1 || port > 65535) {
		error.setStatus(400, "setSockaddr_in: invalid port: '" + port_part + "'");
		return serv_addr;
	}

	
	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(static_cast<uint16_t>(port));
	serv_addr.sin_addr = ipv4_addr;

	return serv_addr;
}

uint32_t ConfigDirectiveParser::validateIPWithSystem(const std::string& ip, int family) {
	uint32_t ipv4_raw;
	if (inet_pton(family, ip.c_str(), &ipv4_raw) != 1) return 0;
	return ntohl(ipv4_raw); 
}

void ConfigDirectiveParser::skipWhitespace(clsParse<TokenType>& parser) {
	while (parser.peek().type == TOKEN_NEW_LINE)
		parser.advance();
}