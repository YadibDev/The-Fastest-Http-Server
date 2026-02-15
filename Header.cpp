#include "Header.hpp"




class ParseHeader{

	private:
		RequestState state;
		std::map<std::string, std::string> &headerMap;

public:

	void	parseHeader(std::string header, int &pos, bool &TheStartOfHeader)
	{
		if (TheStartOfHeader)
		{
			TheStartOfHeader = false;
			if (header.empty())
			{
				state = ERROR;
				return;
			}
		}
	}
}