#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <iomanip>

#include "../../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "../Request/RequestParser.hpp"
#include "../Request/Utils.hpp"
#include "../../RequestHandler/RequestHandler.hpp"
#include "../../RequestHandler/ProcessRequestHandler.hpp"
#include "../Request/HeaderTable.hpp"
#include "../URI/NUriParser.hpp"

// // دالة مساعدة لطباعة نتائج الـ URI بشكل منظم
// void printUriPart(const std::string& label, const s_view& view) {
//     std::cout << "   " << std::left << std::setw(15) << label << ": ";
//     if (view.Data) {
//         print_view(view);
//     } else {
//         std::cout << "\033[1;30m[NULL]\033[0m";
//     }
//     std::cout << std::endl;
// }

// const char* getMethodName(HttpTables::eMethod m) {
//     if (m == HttpTables::M_GET) return "GET";
//     if (m == HttpTables::M_POST) return "POST";
//     if (m == HttpTables::M_DELETE) return "DELETE";
//     return "UNKNOWN";
// }

// void printHeaderSection(const std::string& title) {
//     std::cout << "\n\033[1;35m--- " << title << " ---\033[0m" << std::endl;
// }

// void    test(const char *input, size_t len, s_view &Output, void (*fun)(const char *ptr, size_t len, s_view &Output), std::string nameTest)
// {
//     fun(input, len, Output);
//     printUriPart(nameTest, Output);
// }

// int main() {
//     // 1. قراءة الـ Config
//     std::string configData;
//     int configFd = open("configs/default.conf", O_RDONLY);
//     if (configFd == -1) {
//         std::cerr << "Error: Could not open configs/default.conf" << std::endl;
//         return 1;
//     }
//     HelperFunctions::ReadData(configFd, configData, 10000);
//     close(configFd);

//     LexerConfig<TokenType> cfg(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);
//     cfg.addWithSpace(" \t");
//     cfg.addCommentRule("#", "\n");
//     cfg.addSeparatorToken('{', TOKEN_LBRACE);
//     cfg.addSeparatorToken('}', TOKEN_RBRACE);
//     cfg.addSeparatorToken(';', TOKEN_SEMICOLON);
//     cfg.addSeparatorToken('\n', TOKEN_JOUJNO9ATE);

//     GenericLexer<TokenType> lexer(configData, cfg);
//     std::vector< Token<TokenType> > tokens = lexer.tokenize();
//     clsParse<TokenType> parse(tokens, TOKEN_EOF);
//     clsParseConfigueFile configFile(parse);

//     if (!configFile.ParseConfigue()) {
//         std::cerr << "Config Error: " << configFile.getError().getMsgError() << std::endl;
//         return 1;
//     }
//     std::vector<clsServerConfig> servers = configFile.getServers();

//     clientRoom client;
//     const char *rawRequest =
//         "GET http://mosab:pass123@127.0.0.1:8080/api/v1/users?id=1337#profile HTTP/1.1\r\n"
//         "Host: 127.0.0.1\r\n"
//         "X-Custom: val1\r\n"
//         "X-Custom: val2\r\n"
//         "\r\n";

//     const char *Data = "3933 ";

//     std::memset(client.request_metadata, 0, sizeof(client.request_metadata));
//     std::memcpy(client.request_metadata, rawRequest, std::strlen(rawRequest));

// 	stPollRequest req = makeRequest(client);
//     RequestHandler handler(req);
//     RequestParser parser(req, &servers[0], &handler);
//     parser.init(0);

//     // for (int i = 0; client.request_metadata[i]; i++) {
//         parser.Parse(179);
//         // if (parser.isComplete() || parser.isError()) break;
//     // }

//     if (parser.isError()) {
//         std::cerr << "Parse Error: " << handler.getError().getMsgError() << std::endl;
//         return 1;
//     }

//     if (parser.isComplete()) {
//         std::cout << "\n\033[1;35m--- AUTHORITY & URI (Direct from RequestParser) ---\033[0m" << std::endl;

//         UriParser uri;
//         uri.init(0);
//         s_view Output;
//         HttpError error = uri.;
//         if (error.isError())
//         {
//             std::cout << error.getMsgError() << std::endl;
//             return 1;
//         }
//         // printUriPart("Ipv4", Output);
//         std::cout << "Port : " << uri.getPort() << std::endl;
//         // printUriPart("Host",           uri.getHost());
//         // printUriPart("Port",           uri.getPort());
//         // printUriPart("Path",           uri.getPath());
//         // printUriPart("Query",          uri.getQuery());
//         // printUriPart("Fragment",       uri.getFragment());

//         // std::cout << "\n\033[1;35m--- HEADERS & HANDLER ---\033[0m" << std::endl;
//         // std::cout << "Method:         " << (parser.getRequestLine().getMethod() == HttpTables::M_GET ? "GET" : "OTHER") << std::endl;
//         // std::cout << "Physical Path:  \033[1;32m" << handler.getPhysicalPath() << "\033[0m" << std::endl;
//         // std::cout << "Status Code:    " << handler.getReturn().code << std::endl;
//     }

//     return 0;
// }

#include <iostream>
#include <cstring>
#include <fstream>

// void print_result(RequestParser& parser, const char* input, uint16_t startOffset) {
// 	parser.init(startOffset);
// 	parser.parse(input, std::strlen(input));

// 	RequestLine requestLine = parser.getRequestLine();

// 	std::cout << "Input:  " << input << "\n";
// 	if (parser.isError()) {
// 		std::cout << "Result: ERROR - " << parser.getError().getMsgError() << "\n";
// 	} else {
// 		std::cout << "Result: OK\n";
// 		if (parser.get)
// 			std::cout << "  Authority: " << std::string(parser.getAuthority().Data, parser.getAuthority().len) << "\n";
// 		if (parser.getAuthority().len)
// 			std::cout << "  Host: " << std::string(parser.getHost().Data, parser.getHost().len) << "\n";
// 		if (parser.getPath().len)
// 			std::cout << "  Path:      " << std::string(parser.getPath().Data, parser.getPath().len) << "\n";
// 		if (parser.getQuery().len)
// 			std::cout << "  Query:     " << std::string(parser.getQuery().Data, parser.getQuery().len) << "\n";
// 		if (parser.getFragment().len)
// 			std::cout << "  Fragment:  " << std::string(parser.getFragment().Data, parser.getFragment().len) << "\n";
// 		std::cout << "  Port:      " << parser.getPort() << "\n";
// 	}
// 	std::cout << "----------------------------------------\n";
// }

#include <cstring>

void setPollRequestData(stPollRequest &req, clientRoom &client, const char *raw_request)
{

	std::strncpy(client.request_metadata, raw_request, sizeof(client.request_metadata) - 1);
	client.request_metadata[sizeof(client.request_metadata) - 1] = '\0';

	req.request_metadata = client.request_metadata;
	req.known_headers = client.known_headers;
	req.unknown_headers = client.unknown_headers;
	req.io_chunk = client.io_chunk;
	req.read_body_ptr = &client.read_body;
}

int main()
{

	clientRoom client;
	stPollRequest req;

	const char *http_request =
		"GET /test.py/masin.cpp HTTP/1.1\r\n"
		"Host: 127.0.0.1:8082\r\n"
		"Content-Length: 10\r\n"
		"Connection: keep-alive\r\n"
		"Cache-Control: max-age=0\r\n"
		"sec-ch-ua: \"Not_A Brand\";v=\"8\", \"Chromium\";v=\"120\", \"Brave\";v=\"120\"\r\n"
		"sec-ch-ua-mobile: ?0\r\n"
		"sec-ch-ua-platform: \"macOS\"\r\n"
		"Upgrade-Insecure-Requests: 1\r\n"
		"User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8\r\n"
		"Sec-GPC: 1\r\n"
		"Accept-Language: en-US,en\r\n"
		"Sec-Fetch-Site: none\r\n"
		"Sec-Fetch-Mode: navigate\r\n"
		"Sec-Fetch-User: ?1\r\n"
		"Sec-Fetch-Dest: document\r\n"
		"Accept-Encoding: gzip, deflate, br\r\n"
		"\r\n"
		"abs\0";

	setPollRequestData(req, client, http_request);

	int fd = open("configs/default.conf", O_RDONLY);
	std::string configeData;
	configeData.resize(1025);

	read(fd, &configeData[0], 1024);

	// fd, configeData, 1024

	LexerConfig<TokenType> lexerConfig(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);

	lexerConfig.addSeparatorToken('{', TOKEN_LBRACE);
	lexerConfig.addSeparatorToken('}', TOKEN_RBRACE);
	lexerConfig.addSeparatorToken(';', TOKEN_SEMICOLON);

	lexerConfig.addCommentRule("#", "\n");
	lexerConfig.addWithSpace(" \t\n");

	GenericLexer<TokenType> lexer(configeData, lexerConfig);

	std::vector<Token<TokenType>> Lexer = lexer.tokenize();

	clsParse<TokenType> Data(Lexer, TOKEN_EOF);
	clsParseConfigueFile ConfigueFile(Data);

	ConfigueFile.ParseConfigue();
	if (!ConfigueFile.getServers().size())
	{
		std::cout << ConfigueFile.getError().getMsgError() << std::endl;
		std::cout << ConfigueFile.getError().getCodeStatus() << std::endl;
		std::cout << "Block Server ZERO\n"
				  << std::endl;
		return 1;
	}
	HttpError error;

	clsServerConfig ServerConfig = ConfigueFile.getServers()[0];
	RequestHandler RequestHandler(req);
	RequestParser Parser(req, &RequestHandler);
	Parser.init(&ServerConfig);

	int size = strlen(http_request);
	Parser.Parse(1);
	Parser.Parse(size - 2);
	if (Parser.isComplete())
		std::cout << "\nIsComplete\n\n";

	if (Parser.isError())
	{

		if (!ProcessRequestHandler::generateErrorPath(Parser.getError().getCodeStatus(), &ServerConfig, &RequestHandler, error))
		{
			RequestHandler.setDefaultErrorPage(true);
			std::cout << "Default Error Page\n";
		}
	}
	RequestLine requestLine = Parser.getRequestLine();

	std::string Method = (requestLine.getMethod() == 0) ? "GET" : "POST";

	std::cout << "Method : " << Method << std::endl;
	std::cout << "PhysicalPath : " << RequestHandler.getPhysicalPath() << std::endl;
	std::cout << "Return code : " << RequestHandler.getReturn().code << std::endl;
	std::cout << "Return value : " << RequestHandler.getReturn().value.raw_path << std::endl;
	std::cout << "Name Script : ";
	print_view(RequestHandler.getScriptName());
	std::cout << std::endl;
	std::cout << "Path Info : ";
	print_view(RequestHandler.getPathInfo());
	std::cout << std::endl;
	std::cout << "Path Translated : " << &RequestHandler.getPathTranslated()[0] << std::endl;
	std::string cgi = (!RequestHandler.getPathCgi()) ? "NULL" : *RequestHandler.getPathCgi();
	std::cout << "Path Cgi : " << cgi << std::endl;
	std::cout << "code Error : " << RequestHandler.getStatusError() << std::endl;
	std::cout << "Default Error Page : "
			  << (RequestHandler.getDefaultErrorPage() ? "(YES)" : "(NULL)")
			  << std::endl;

	HeaderTable headerTable = RequestHandler.getHeader();
	const s_header_slot *Host = headerTable.getKnownHeader(HttpTables::H_HOST);
	const s_header_slot *Content_length = headerTable.getKnownHeader(HttpTables::H_CONTENT_LENGTH);
	std::cout << "Host : ";
	print_view(Host->val);
	std::cout << std::endl;
	std::cout << "Content_length : ";
	print_view(Content_length->val);
	std::cout << std::endl;

	return 0;
}