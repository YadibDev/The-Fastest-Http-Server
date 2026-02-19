#include "ConfigDirectiveParser.hpp"

bool ConfigDirectiveParser::parseLocationPath(s_parse_context& ctx, stlocation& loc) {

	std::string token = ctx.parser.peek().value;

	if (token == "=") {
		loc.matchType = stlocation::EXACT;
		ctx.parser.advance();
	} else if (token == "^~") {
		loc.matchType = stlocation::PREFIX;
		ctx.parser.advance();
	}
	else
		loc.matchType = stlocation::PREFIX;

	if (ctx.parser.peek().type != TOKEN_WORD)
		return (ctx.error.setStatus(400, "Location Error: Missing URI path"), false);

	loc.uri = ctx.parser.peek().value;

	if (loc.uri[0] == '~')
		return (ctx.error.setStatus(400, "Location Error: Regex is not supported"), false);

	ctx.parser.advance();

	if (ctx.parser.peek().type != TOKEN_LBRACE)
		return (ctx.error.setStatus(400, "Location Error: Expected '{' after path"), false);

	ctx.parser.advance();
	skipWhitespace(ctx);

	return true;
}

std::string ConfigDirectiveParser::ParseRoot(s_parse_context& ctx) {
	ctx.parser.advance(); 
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (ctx.error.setStatus(400, "Expected path after 'root'"), "");

	std::string root;
	ctx.error = URIParser::normalizePath(ctx.parser.peek().value, root);
	if (ctx.error.isError()) return "";

	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Missing ';' after root"), "");
	
	ctx.parser.advance();
	skipWhitespace(ctx);

	return root;
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
	skipWhitespace(ctx);

	return bytes;
}

std::vector<std::string> ConfigDirectiveParser::ParseIndex(s_parse_context& ctx) {
	std::vector<std::string> indices;
	ctx.parser.advance();

	while (ctx.parser.peek().type == TOKEN_WORD) {
		indices.push_back(ctx.parser.peek().value);
		ctx.parser.advance();
	}

	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Missing ';' after index list"), indices);
	
	ctx.parser.advance();
	skipWhitespace(ctx);
	return indices;
}

bool ConfigDirectiveParser::ParseAutoIndex(s_parse_context& ctx) {
	ctx.parser.advance();
	bool result = false;
	std::string val = ctx.parser.peek().value;

	if (val == "on") result = true;
	else if (val == "off") result = false;
	else ctx.error.setStatus(400, "Invalid autoindex value: " + val);

	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Missing ';' after autoindex"), false);
	
	ctx.parser.advance();
	skipWhitespace(ctx);

	return result;
}

sockaddr_in ConfigDirectiveParser::ParseListen(s_parse_context& ctx) {
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(sockaddr_in));
	ctx.parser.advance();

	if (ctx.parser.peek().type == TOKEN_WORD) {
		addr = setSockaddr_in(ctx.parser.peek().value);
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
	skipWhitespace(ctx);
	return addr;
}

stReturnData ConfigDirectiveParser::ParseReturn(s_parse_context& ctx) {
	stReturnData ReturData;
	ctx.parser.advance();
	char* end;
	
	ReturData.code = std::strtol(ctx.parser.peek().value.c_str(), &end, 10);
	if (*end != '\0') ReturData.code = -1;

	if (ctx.parser.advance().type != TOKEN_WORD) {
		ctx.error.setStatus(400, "Syntax Error: Expected URI after return code");
		return ReturData;
	}
	ReturData.value = ctx.parser.peek().value;

	if (ctx.parser.advance().type != TOKEN_SEMICOLON) {
		ctx.error.setStatus(400, "Syntax Error: Missing ';' after return");
	}
	ctx.parser.advance();
	skipWhitespace(ctx);
	return ReturData;
}

std::string ConfigDirectiveParser::ParseUploadStore(s_parse_context& ctx) {
	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_WORD)
		return (ctx.error.setStatus(400, "Expected path after 'upload_path'"), "");

	std::string path = ctx.parser.peek().value;
	if (!isValidPath(ctx, path, true))
		return (ctx.error.setStatus(400, "Invalid upload path -> " + path), "");
	
	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		ctx.error.setStatus(400, "Missing ';' after upload_path");

	ctx.parser.advance();
	skipWhitespace(ctx);
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

	if (!isValidPath(ctx, binPath, false)) return;

	ctx.parser.advance();
	if (ctx.parser.peek().type != TOKEN_SEMICOLON) {
		ctx.error.setStatus(400, "CGI Error: Expected ';'");
		return;
	}
	ctx.parser.advance();
	skipWhitespace(ctx);
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
	skipWhitespace(ctx);
	return combinedMethods;
}

std::map<short, stErrorPagedata> ConfigDirectiveParser::ParseErrorPage(s_parse_context& ctx) {
	std::map<short, stErrorPagedata> errorMap;
	std::vector<short> codes;
	short responseOverride = 0;
	std::string uri = "";

	ctx.parser.advance();

	while (ctx.parser.peek().type == TOKEN_WORD && HelperFunctions::is_numeric(ctx.parser.peek().value))
	{
		codes.push_back((short)std::atoi(ctx.parser.peek().value.c_str()));
		ctx.parser.advance();
	}

	if (ctx.parser.peek().type == TOKEN_WORD && ctx.parser.peek().value[0] == '=')
	{
		std::string resStr = ctx.parser.peek().value;
		if (resStr.size() > 1)
			responseOverride = (short)std::atoi(resStr.substr(1).c_str());
		else
			responseOverride = -1; 
		ctx.parser.advance();
	}

	if (ctx.parser.peek().type == TOKEN_WORD)
	{
		uri = ctx.parser.peek().value;
		ctx.parser.advance();
	}
	else
		return (ctx.error.setStatus(400, "Syntax Error: Missing URI in error_page"), errorMap);

	if (ctx.parser.peek().type != TOKEN_SEMICOLON)
		return (ctx.error.setStatus(400, "Syntax Error: Missing ';' after error_page"), errorMap);
	
	ctx.parser.advance();
	skipWhitespace(ctx);

	stErrorPagedata data;
	data.response = responseOverride;
	data.uri = uri;

	for (size_t i = 0; i < codes.size(); ++i)
		errorMap[codes[i]] = data;

	return errorMap;
}



// Private Helpers
bool ConfigDirectiveParser::isValidPath(s_parse_context& ctx, const std::string& path, bool expectDir) {
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return (ctx.error.setStatus(400, "Path Error: Not found -> " + path), false);

	if (expectDir && !S_ISDIR(info.st_mode))
		return (ctx.error.setStatus(400, "Path Error: Expected directory -> " + path), false);

	if (!expectDir && S_ISDIR(info.st_mode))
		return (ctx.error.setStatus(400, "Path Error: Expected file -> " + path), false);

	return true;
}

unsigned long long ConfigDirectiveParser::convertToBytes(long long value, char unit, HttpError& error) {
	unsigned long long multiplier = 1;
	switch (unit) {
		case 'g': case 'G': multiplier = 1024ULL * 1024 * 1024; break;
		case 'm': case 'M': multiplier = 1024ULL * 1024; break;
		case 'k': case 'K': multiplier = 1024ULL; break;
		case ' ': multiplier = 1; break;
		default: error.setStatus(400, "Invalid unitType"); return 0;
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

sockaddr_in ConfigDirectiveParser::setSockaddr_in(const std::string& input) {
	unsigned short port = 0;
	std::string host = "";
	struct sockaddr_in serv_addr;
	std::memset(&serv_addr, 0, sizeof(serv_addr));

	URIParser::extractPort(input, port);
	URIParser::extractHost(input, host);

	if (host == "*" || host == "") host = "0.0.0.0";
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	uint32_t ipv4 = validateIPWithSystem(host, AF_INET);
	serv_addr.sin_addr.s_addr = htonl(ipv4);

	return serv_addr;
}

uint32_t ConfigDirectiveParser::validateIPWithSystem(const std::string& ip, int family) {
	uint32_t ipv4_raw;
	if (inet_pton(family, ip.c_str(), &ipv4_raw) != 1) return 0;
	return ntohl(ipv4_raw); 
}

void ConfigDirectiveParser::skipWhitespace(s_parse_context& ctx) {
	while (ctx.parser.peek().type == TOKEN_JOUJNO9ATE)
		ctx.parser.advance();
}