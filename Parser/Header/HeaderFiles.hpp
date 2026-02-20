#ifndef HEADERFILES_HPP
#define HEADERFILES_HPP

#include "../../Utils/Lexer.hpp"
#include "../../Utils/HelperFunctions.hpp"
#include "../../Utils/HttpError.hpp"
#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
#include "../ParseRequest/URI/URI.hpp"
#include <map>
#include <string>
#include <vector>


enum eMethods { GET = 1, POST = 2, DELETE = 4 };

enum RequestState {
	READING_LINE,
	READING_HEADERS,
	READING_BODY,
	COMPLETED,
	ERROR
};

struct  stArguments {
	size_t        _Pos;
	HttpError    _Error;
	std::string    _Data;
	RequestState _State;
};

#endif