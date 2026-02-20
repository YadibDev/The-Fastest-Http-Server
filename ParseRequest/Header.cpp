#include "Header.hpp"

#include "Header.hpp"


// implementation of the class declared in Header.hpp

ParseHeader::ParseHeader(std::map<std::string, std::vector<std::string> >& headers)
	: state(READING_HEADERS), headerMap(headers) {}

bool ParseHeader::checkHeaderField(std::string &HeaderField)
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

bool ParseHeader::parseSingleHeader(const std::string& line, HttpError& error)
{
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos)
        return (error.setStatus(400, "Invalid Header Field: Missing colon"), false);

    std::string headerField = line.substr(0, colonPos);
    if (!checkHeaderField(headerField))
        return (error.setStatus(400, "Invalid Header Field Name"), false);

    std::string fieldValue = line.substr(colonPos + 1);
    fieldValue = HelperFunctions::normalizeLWS(fieldValue);

    storeHeader(headerField, fieldValue, headerMap);

    return true;
}



std::vector<std::string> ParseHeader::getHeaderValues(const std::string& headerField) const
{
	std::map<std::string, std::vector<std::string> >::const_iterator it = headerMap.find(headerField);
	if (it != headerMap.end())
		return it->second;
	return std::vector<std::string>();
}