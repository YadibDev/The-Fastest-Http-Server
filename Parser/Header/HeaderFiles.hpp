#ifndef HEADERFILES_HPP
#define HEADERFILES_HPP

#include "../../Utils/Lexer.hpp"
#include "../../Utils/HelperFunctions.hpp"
#include "../../Utils/HttpError.hpp"
#include "../ParseRequest/URI/URI.hpp"
#include "../ParseRequest/URI/NUriParser.hpp"
#include "../ParseRequest/Request/HttpTypes.hpp"



#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>


struct UriStatus {
	bool is_relative   : 1;
	bool is_abs_path   : 1;
	bool is_cgi        : 1;
	bool is_abs_uri    : 1;
	bool is_dir        : 1;
	bool is_file       : 1;
	bool is_symlink    : 1;

	bool can_read      : 1;
	bool can_write     : 1;
	bool can_execute   : 1;

	bool exists        : 1;
};

struct sPathType {
	enum e_path_type {
	    PATH_NOT_FOUND = 0,
	    PATH_FILE      = 1,
	    PATH_DIR       = 2,
	    PATH_OTHER     = 3
	};
};

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

struct s_uri_entry {
	std::string     raw_path;
	UriStatus       flags;
	s_view          sv_raw_path;
	uint8_t         redirect_count;

	s_uri_entry() : raw_path("")
	{
		memset(&flags, 0, sizeof(flags));
		sv_raw_path.Data = NULL;
		sv_raw_path.len = 0;
		redirect_count = 0;
	}

	const std::string& getPath() const { return raw_path; }

	void initView() {
		if (!raw_path.empty()) {
			sv_raw_path.Data = &raw_path[0];
			sv_raw_path.len = raw_path.size();
		}
	}

	void	setSview(s_view &sv_path)
	{
		sv_raw_path = sv_path;
	}

	void AddRedirectCount() { redirect_count++; }
	s_view getView() const { return sv_raw_path; }
};

struct stReturnData {    
	short       code;
	s_uri_entry value;

	stReturnData() : code(-1) { }
};

struct stErrorPagedata {
	short       response;
	s_uri_entry uri;

	stErrorPagedata() : response(-1){ }
};

#endif


// #include "../ParseConfigFile/ConfigFile/ParseConfigueFile.hpp"
// #include "../ParseRequest/URI/URI.hpp"