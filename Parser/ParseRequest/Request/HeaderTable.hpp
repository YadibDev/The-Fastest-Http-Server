#ifndef HEADERTABLE_HPP
#define HEADERTABLE_HPP

#include "HttpTypes.hpp"
#include <cstring>

class HeaderTable {
private:
	stPollRequest& _request;

	bool	_keysMatch(const s_view& k1, const s_view& k2) const;

public:
	HeaderTable(stPollRequest& req);
	~HeaderTable();
	HeaderTable& operator=(const HeaderTable& rhs);
	// modified by achraf here
	// const s_header_slot*	getKnownHeader(HttpTables::eKnownHeader h) ;
	const s_header_slot*	getKnownHeader(HttpTables::eKnownHeader h) const;
	const s_header_slot*	getUnknownHeader(uint8_t index) const;

	void	linkThisHeader();

	bool	isDuplicate(uint8_t index);
	uint8_t	countOccurrences(HttpTables::eKnownHeader h);

};

#endif