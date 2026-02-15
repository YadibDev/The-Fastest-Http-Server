#include "Header.hpp"




class ParseHeader{

	private:
		RequestState state;
		std::map<std::string, std::string> &headerMap;

		bool	checkHeaderField(std::string HeaderField)
		{
			if (HeaderField.empty() || HeaderField.back() != ':')
				return (false);
			if (HelperFunctions::is_CTLsString(HeaderField) || HelperFunctions::is_numeric(HeaderField))
				return (false);
			return (true);
		}

		bool	getValue(stArguments args, std::string &fieldValue)
		{
			size_t posFieldValue = args._Data.find("\r\n", args._Pos);
			if (posFieldValue == std::string::npos)
				return (args._Error.setStatus(400, "Bad Request"), false);
			
			if (args._Data[posFieldValue + 2] != ' ' || args._Data[posFieldValue + 2] != '\t')
			{
				fieldValue = args._Data.substr(args._Pos, posFieldValue - args._Pos);
				fieldValue = HelperFunctions::trim(fieldValue);
				return (true);
			}
			if ()
		}

public:

	bool	parseHeader(stArguments args, bool &TheStartOfHeader)
	{
		if (TheStartOfHeader)
		{
			TheStartOfHeader = false;
			if (args._Data.empty() || args._Data[0] == ' ' || args._Data[0] == '\t')
				return (args._Error.setStatus(400, "Bad Request"), false);
		}
		while (args._Data.size() > args._Pos)
		{
			size_t fieldName = args._Data.find(" ", args._Pos);
			if (fieldName == std::string::npos)
				return (args._Error.setStatus(400, "Bad Request"), false);
			if (!checkHeaderField(args._Data.substr(args._Pos, fieldName - args._Pos)))
				return (args._Error.setStatus(400, "Bad Request"), false);
			args._Pos = fieldName + 1;
			
		}
		return (true);
	}
};