#define PARSELOCATIONCONFIG_HPP
#ifndef PARSELOCATIONCONFIG_HPP

#include "LocationConfigData.hpp"
#include "../../Utils/HttpError.hpp"
#include <iostream>

class clsParseLocationConfig
{
	private:
		enum RequestState {
			READING_LINE,
			READING_HEADERS,
			READING_BODY,
			COMPLETED,
			ERROR
		};

		clsLocationConfig	LocationConfig;
		enum RequestState	State;

		HttpError parseRequestLine ();
	public:
		clsParseLocationConfig() : _state(READING_LINE);
		HttpError ParseRequest(std::string &rawData);
	
}



#endif