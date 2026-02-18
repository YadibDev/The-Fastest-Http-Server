#include <string>
#include <map>
#include <vector>
#include "URI.hpp"
#include "../Utils/HelperFunctions.hpp"
#include "../Utils/HttpError.hpp"
#include "Request.hpp"
#include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"



enum eMethods { GET = 1, POST = 2, DELETE = 4 };

enum RequestState {
	READING_LINE,
	READING_HEADERS,
	READING_BODY,
	COMPLETED,
	ERROR
};

struct  stArguments {
	size_t		_Pos;
	HttpError	_Error;
	std::string	_Data;
};

