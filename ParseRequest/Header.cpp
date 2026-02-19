#include "Header.hpp"

#include "Header.hpp"


// implementation of the class declared in Header.hpp

ParseHeader::ParseHeader(std::map<std::string, std::vector<std::string> >& headers)
	: state(READING_HEADERS), headerMap(headers) {}

bool ParseHeader::checkHeaderField(std::string HeaderField)
{
	if (HeaderField.empty() || HelperFunctions::myIsspace(HeaderField, 0))
		return (false);
	if (HelperFunctions::is_CTLsString(HeaderField) || HelperFunctions::is_numeric(HeaderField))
		return (false);
	return (true);
}

bool ParseHeader::getValue(stArguments &args, std::string &fieldValue)
{
	size_t posFieldValue = args._Data.find("\r\n", args._Pos);
	if (posFieldValue == std::string::npos)
		return (args._Error.setStatus(400, "Invalid Field Value"), false);
	posFieldValue += 2; // Move past the "\r\n"

	if (args._Data.size() > posFieldValue && ((args._Data[posFieldValue] == ' ' || args._Data[posFieldValue] == '\t')))
	{
		fieldValue += " " + args._Data.substr(args._Pos, posFieldValue - args._Pos - 2);
		fieldValue = HelperFunctions::normalizeLWS(fieldValue);
		args._Pos = posFieldValue;
		return (getValue(args, fieldValue));
	}
	fieldValue += " " + args._Data.substr(args._Pos, posFieldValue - args._Pos - 2);
	fieldValue = HelperFunctions::normalizeLWS(fieldValue);
	args._Pos = posFieldValue;
	return (true);
}

 bool ParseHeader::checkDoubleCRLF(std::string &data, size_t pos)
{
	char DoubleCRLF[] = "\r\n";

	if (data.size() >= pos + 2 && !data.compare(pos, 2, DoubleCRLF))
		return (true);
	return (false);
}

void ParseHeader::storeHeader(std::string &headerField, std::string &fieldValue, std::map<std::string, std::vector<std::string> > &headerMap)
{
	std::vector<std::string> values;
	values = HelperFunctions::splitCommaSeparated(fieldValue);

	for (size_t i = 0; i < values.size(); i++)
		headerMap[headerField].push_back(values[i]);

	fieldValue.clear();
	headerField.clear();
}

bool ParseHeader::parseHeader(stArguments &args, bool &TheStartOfHeader)
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
		size_t fieldName = args._Data.find(":", args._Pos);
		if (fieldName == std::string::npos)
			return (args._Error.setStatus(400, "Not found Space in fieldName"), false);

		headerField = args._Data.substr(args._Pos, fieldName - args._Pos);
		if (!checkHeaderField(headerField))
			return (args._Error.setStatus(400, "Invalid Header Field"), false);
		args._Pos = fieldName + 1;
		if (!getValue(args, fieldValue))
			return (false);
		storeHeader(headerField, fieldValue, headerMap);
		if (args._Data.size() > args._Pos && args._Data[args._Pos] == '\r')
		{
			if (checkDoubleCRLF(args._Data, args._Pos))
			{
				args._Pos += 4;
				args._State = READING_BODY;
				return (true);
			}
			else
				return (args._Error.setStatus(400, "Invalid Header Format"), false);
		}
	}
	return (true);
}

