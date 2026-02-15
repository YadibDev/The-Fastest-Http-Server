#include "Header.hpp"

#include "Header.hpp"


// implementation of the class declared in Header.hpp

ParseHeader::ParseHeader(std::map<std::string, std::string>& headers)
	: state(READING_HEADERS), headerMap(headers) {}

bool ParseHeader::checkHeaderField(std::string HeaderField)
{
	if (HeaderField.empty() || HeaderField.back() != ':')
		return (false);
	if (HelperFunctions::is_CTLsString(HeaderField) || HelperFunctions::is_numeric(HeaderField))
		return (false);
	return (true);
}

bool ParseHeader::getValue(stArguments args, std::string &fieldValue)
{
	size_t posFieldValue = args._Data.find("\r\n", args._Pos);
	if (posFieldValue == std::string::npos)
		return (args._Error.setStatus(400, "Invalid Field Value"), false);
	posFieldValue += 2; // Move past the "\r\n"
	if (args._Data.size() <= posFieldValue + 1 && ((args._Data[posFieldValue + 1] != ' ' || args._Data[posFieldValue + 1] != '\t')))
	{
		fieldValue += " " + args._Data.substr(args._Pos, posFieldValue - args._Pos - 2);
		fieldValue = HelperFunctions::normalizeLWS(fieldValue);
		args._Pos = posFieldValue;
		return (true);
	}
	posFieldValue = args._Data.find("\r\n", args._Pos);
	if (posFieldValue != std::string::npos)
	{
		if (args._Data.size() <= posFieldValue + 1 && ((args._Data[posFieldValue + 1] != ' ' || args._Data[posFieldValue + 1] != '\t')))
		{
			fieldValue += " " + args._Data.substr(args._Pos, posFieldValue - args._Pos - 2);
			fieldValue = HelperFunctions::normalizeLWS(fieldValue);
			args._Pos = posFieldValue + 2;
			return (getValue(args, fieldValue));
		}
		else
		{
			fieldValue += " " + args._Data.substr(args._Pos, posFieldValue - args._Pos - 2);
			fieldValue = HelperFunctions::normalizeLWS(fieldValue);
			args._Pos = posFieldValue + 2;
			return (true);
		}
	}
	return (args._Error.setStatus(400, "Invalid Field Value"), false);
}

bool ParseHeader::parseHeader(stArguments args, bool &TheStartOfHeader)
{
	std::string fieldValue = "";
	std::string headerField = "";

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
			return (args._Error.setStatus(400, "Not found Space in fieldName"), false);
		headerField = args._Data.substr(args._Pos, fieldName - args._Pos);
		if (!checkHeaderField(headerField))
			return (args._Error.setStatus(400, "Invalid Header Field"), false);
		args._Pos = fieldName + 1;
		if (!getValue(args, fieldValue))
			return (false);
		headerMap[headerField] = fieldValue;
	}
	return (true);
}

