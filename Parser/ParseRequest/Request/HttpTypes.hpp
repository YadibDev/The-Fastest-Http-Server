#ifndef HTTP_TYPES_HPP
#define HTTP_TYPES_HPP

#include <stdint.h>
#include <string>
#include <vector>

#define INVALID_INDEX 255
#define SIZE_BUFFER 30000
// #define SIZE_BUFFER 16384

#define SIZE_UNKNOW_HEADER 25
	
struct s_view
{
	char *Data;
	uint16_t len;
	s_view() : Data(NULL), len(0) {}

	void reset()
	{
		Data = NULL;
		len = 0;
	}
};

class HttpTables {
public:

	enum eMethod { M_GET, M_POST, M_DELETE, M_METHOD_COUNT, M_UNKNOWN };
	enum State { STATE_KEY, STATE_VALUE, STATE_CR, STATE_LF, STATE_DECISION, STATE_COMPLETE, STATE_ERROR };
	enum eKnownHeader {
		H_HOST, H_CONTENT_LENGTH, H_TRANSFER_ENCODING, H_CONTENT_TYPE,
		H_CONNECTION, H_COOKIE, H_COUNT, H_UNKNOWN
	};

	static const char **methods() {
		static const char *arr[M_METHOD_COUNT] = { "GET", "POST", "DELETE" };
		return arr;
	}

	static const char **headers() {
		static const char *arr[H_COUNT] = {
			"Host", "Content-Length", "Transfer-Encoding", "Content-Type",
			"Connection", "Cookie"
		};
		return arr;
	}
};

struct s_header_slot
{
	s_view		key;
	s_view		val;
	uint8_t		next;
	int32_t		Hash;

	s_header_slot() : next(INVALID_INDEX), Hash(-1) {}

	void reset()
	{
		key.reset();
		val.reset();
		next = INVALID_INDEX;
		Hash = -1;
	}
};

struct PollOfClient
{
	char			request_metadata[SIZE_BUFFER];
	char			io_chunk[SIZE_BUFFER];
	s_header_slot	known_headers[HttpTables::H_COUNT];
	s_header_slot	unknown_headers[SIZE_UNKNOW_HEADER];
	char			Response_metadata[SIZE_BUFFER];
	uint16_t		read_offset;
	uint16_t read_body;
	void Reset()
	{
		read_offset = 0;
		read_body = 0;

		for (int i = 0; i < HttpTables::H_COUNT; ++i)
			known_headers[i].reset();

		for (int i = 0; i < SIZE_UNKNOW_HEADER; ++i)
			unknown_headers[i].reset();
	}
};

struct stPollRequest {
	char			*request_metadata;
	s_header_slot	*known_headers;
	s_header_slot	*unknown_headers;
	char			*io_chunk;
	uint16_t		*read_body_ptr;
};

#endif