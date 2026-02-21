#ifndef HEADERFILES_HPP
#define HEADERFILES_HPP

#include "../../Utils/Lexer.hpp"
#include "../../Utils/HelperFunctions.hpp"
#include "../../Utils/HttpError.hpp"
#include "../ParseRequest/URI/URI.hpp"



#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>


enum eMethods { GET = 1, POST = 2, DELETE = 4};

enum RequestState {
	READING_LINE,
	READING_HEADERS,
	READING_BODY,
	COMPLETED,
	ERROR = 100
};

struct  stArguments {
	size_t        _Pos;
	HttpError    _Error;
	std::string    _Data;
	RequestState _State;
};


struct stlocation {
	enum eType {
		EXACT,
		PREFIX
	};

	eType		matchType;
	std::string uri;
};

struct stReturnData {    
	short       code;
	std::string value;

	stReturnData() : code(-1), value("") {}    
};

struct stErrorPagedata {
	short       response;
	std::string uri;

	stErrorPagedata() : response(0), uri("") {}
};

#endif


// #include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
// #include "../ParseRequest/URI/URI.hpp"