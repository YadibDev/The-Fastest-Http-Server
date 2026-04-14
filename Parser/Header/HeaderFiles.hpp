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


struct Directives {
	enum e_directive_flags {
	    D_NONE        	= 0,
	    D_ROOT        	= 1 << 1,
	    D_ALIAS       	= 1 << 2,
	    D_MAX_BODY    	= 1 << 3,
	    D_AUTOINDEX   	= 1 << 4,
	    D_LISTEN      	= 1 << 5,
	    D_RETURN      	= 1 << 6,
	    D_METHODS		= 1 << 7,
		D_UPLOAD_STORE	= 1 << 8
	};
};

struct Methods {
	enum eMethods 
	{
		GET = 1,
		POST = 2,
		DELETE = 4
	};
};

struct RequestStatus {
    enum e_state {
        READING_LINE    = 0,
        READING_HEADERS = 1,
        READING_BODY    = 2,
        COMPLETED       = 3,
        ERROR           = 4
    };
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

	stErrorPagedata() : response(-1), uri("") {}
};

#endif


// #include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
// #include "../ParseRequest/URI/URI.hpp"