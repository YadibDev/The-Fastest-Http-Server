#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <iomanip>

// Includes الخاصة بالمشروع
#include "../../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "../Request/RequestParser.hpp"
#include "../Request/Utils.hpp"
#include "../../RequestHandler/RequestHandler.hpp"
#include "../../RequestHandler/ProcessRequestHandler.hpp"
#include "../Request/HeaderTable.hpp"



const char* getMethodName(HttpTables::eMethod m) {
    if (m == HttpTables::M_GET) return "GET";
    if (m == HttpTables::M_POST) return "POST";
    if (m == HttpTables::M_DELETE) return "DELETE";
    return "UNKNOWN";
}

void printHeaderSection(const std::string& title) {
    std::cout << "\n\033[1;35m--- " << title << " ---\033[0m" << std::endl;
}

int main() {
    // 1. قراءة وتحليل ملف الإعدادات (Config)
    std::string configData;
    int configFd = open("configs/default.conf", O_RDONLY);
    if (configFd == -1) {
        std::cerr << "Error: Could not open configs/default.conf" << std::endl;
        return 1;
    }
    HelperFunctions::ReadData(configFd, configData, 10000);
    close(configFd);

    LexerConfig<TokenType> cfg(TOKEN_WORD, TOKEN_EOF, TOKEN_NULL);
    cfg.addWithSpace(" \t"); // تبسيط لإعدادات الـ Lexer
    
    cfg.addCommentRule("#", "\n");
    cfg.addSeparatorToken('{', TOKEN_LBRACE);
    cfg.addSeparatorToken('}', TOKEN_RBRACE);
    cfg.addSeparatorToken(';', TOKEN_SEMICOLON);
    cfg.addSeparatorToken('\n', TOKEN_JOUJNO9ATE);

    GenericLexer<TokenType> lexer(configData, cfg);
    std::vector< Token<TokenType> > tokens = lexer.tokenize();
    clsParse<TokenType> parse(tokens, TOKEN_EOF);
    clsParseConfigueFile configFile(parse);

    if (!configFile.ParseConfigue()) {
        std::cerr << "Config Error: " << configFile.getError().getMsgError() << std::endl;
        return 1;
    }
    std::vector<clsServerConfig> servers = configFile.getServers();

    PollOfClient client;
    const char *rawRequest =
        "POST /cgi/script.py/path HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "X-Custom: value1\r\n"
        "X-Custom: value2\r\n" // هيدر مكرر لاختبار countOccurrences
        "Content-Length: 10\r\n"
        "\r\n"
        "BodyData!!";

    std::memset(client.request_metadata, 0, sizeof(client.request_metadata));
    std::memcpy(client.request_metadata, rawRequest, std::strlen(rawRequest));

    stPollRequest req = makeRequest(client);
    RequestHandler handler(req); 
    RequestParser parser(req, &servers[0], &handler);
    parser.init(0);

    for (int i = 0; client.request_metadata[i]; i++) {
        parser.Parse(i);
        if (parser.isComplete() || parser.isError()) break;
    }

    if (parser.isError()) {
        std::cerr << "Parse Error: " << handler.getError().getMsgError() << std::endl;
        return 1;
    }

    if (parser.isComplete()) {
        HeaderTable& table = handler.getHeader();

        printHeaderSection("Testing getKnownHeader");
        s_header_slot* hostSlot = table.getKnownHeader(HttpTables::H_HOST);
        if (hostSlot && hostSlot->val.Data) {
            std::cout << "Host found: ";
            print_view(hostSlot->val);
            std::cout << std::endl;
        }

        printHeaderSection("Testing getUnknownHeader & isDuplicate");
        s_header_slot* unknownSlot = table.getUnknownHeader(0);
        if (unknownSlot && unknownSlot->key.Data) {
            std::cout << "First Unknown Key: ";
            print_view(unknownSlot->key);
            std::cout << " | Value: ";
            print_view(unknownSlot->val);
            std::cout << "\nIs Duplicate: " << (table.isDuplicate(0) ? "Yes" : "No") << std::endl;
        }

        printHeaderSection("Testing countOccurrences");
            int foundCount = 0;
            for (uint8_t i = 0; i < 25; ++i) {
                s_header_slot* slot = table.getUnknownHeader(i);

                if (slot && slot->val.Data != NULL) {
                    foundCount++;
                    std::cout << "\033[1;36m[Slot " << (int)i << "]\033[0m" << std::endl;

                    std::cout << "   Key:   "; print_view(slot->key); std::cout << std::endl;
                    std::cout << "   Value: "; print_view(slot->val); std::cout << std::endl;

                    bool dup = table.isDuplicate(i);
                    std::cout << "   Status: " << (dup ? "\033[1;31mDuplicate (Linked)\033[0m" : "\033[1;32mOriginal\033[0m") << std::endl;

                    if (slot->next != HttpTables::INVALID_INDEX) {
                        std::cout << "   Next Link -> Slot [" << (int)slot->next << "]" << std::endl;
                    }
                    std::cout << "------------------------------------\n";
                }
            }

            if (foundCount == 0) {
                std::cout << "No unknown headers found. Check your Parser logic!\n";
            } else {
                std::cout << "\033[1;32mTotal Unknown Headers in Array: " << foundCount << "\033[0m\n";
            }
            } else {
            std::cerr << "Parser failed to complete the request.\n";
        }
        printHeaderSection("Final Handler Decisions");
        std::cout << "Method:         " << getMethodName(handler.getMethod()) << std::endl;
        std::cout << "Physical Path:  \033[1;32m" << handler.getPhysicalPath() << "\033[0m" << std::endl;
        std::cout << "CGI: " << *handler.getPathCgi() << std::endl;
        std::cout << "PathInfo: "; print_view(handler.getPathInfo()); std::cout << std::endl;
        std::cout << "PathTranslated: "<< handler.getPathTranslated() << std::endl;
        std::cout << "Status Code:    " << handler.getReturn().code << std::endl;
    return 0;
}