#include "Header.hpp"



// // implementation of the class declared in Header.hpp

// ParseHeader::ParseHeader(std::map<std::string, std::vector<std::string> >& headers)
// 	: state(RequestStatus::READING_HEADERS), headerMap(headers) {}

// bool ParseHeader::checkHeaderField(std::string &HeaderField)
// {
// 	if (HeaderField.empty() || HelperFunctions::myIsspace(HeaderField, 0))
// 		return (false);
// 	if (HelperFunctions::is_CTLsString(HeaderField) || HelperFunctions::is_numeric(HeaderField))
// 		return (false);
// 	return (true);
// }

// bool ParseHeader::getValue(stArguments &args, std::string &fieldValue)
// {
// 	size_t posFieldValue = args._Data.find("\r\n", args._Pos);
// 	if (posFieldValue == std::string::npos)
// 		return (args._Error.setStatus(400, "Invalid Field Value"), false);
// 	posFieldValue += 2; // Move past the "\r\n"

// 	if (args._Data.size() > posFieldValue && ((args._Data[posFieldValue] == ' ' || args._Data[posFieldValue] == '\t')))
// 	{
// 		fieldValue += " " + args._Data.substr(args._Pos, posFieldValue - args._Pos - 2);
// 		fieldValue = HelperFunctions::normalizeLWS(fieldValue);
// 		args._Pos = posFieldValue;
// 		return (getValue(args, fieldValue));
// 	}
// 	fieldValue += " " + args._Data.substr(args._Pos, posFieldValue - args._Pos - 2);
// 	fieldValue = HelperFunctions::normalizeLWS(fieldValue);
// 	args._Pos = posFieldValue;
// 	return (true);
// }

//  bool ParseHeader::checkDoubleCRLF(std::string &data, size_t pos)
// {
// 	char DoubleCRLF[] = "\r\n";

// 	if (data.size() >= pos + 2 && !data.compare(pos, 2, DoubleCRLF))
// 		return (true);
// 	return (false);
// }

// void ParseHeader::storeHeader(std::string &headerField, std::string &fieldValue, std::map<std::string, std::vector<std::string> > &headerMap)
// {
// 	std::vector<std::string> values;
// 	values = HelperFunctions::splitCommaSeparated(fieldValue);

// 	for (size_t i = 0; i < values.size(); i++)
// 		headerMap[HelperFunctions::ConvertStringToLower(headerField)].push_back(values[i]);

// 	fieldValue.clear();
// 	headerField.clear();
// }


// bool ParseHeader::parseSingleHeader(const std::string& line, HttpError& error)
// {
//     size_t colonPos = line.find(':');
//     if (colonPos == std::string::npos || colonPos == 0)
//         return (error.setStatus(400, "Bad Request: Malformed Header"), false);

//     std::string fieldName = HelperFunctions::ConvertStringToLower(line.substr(0, colonPos));
    
//     if (isSingleton(fieldName)) {
//         if (hasAlreadyBeenSet(fieldName))
//             return (error.setStatus(400, "Bad Request: Duplicate Singleton Header"), false);
//         setFlag(fieldName);
//     }

//     std::string fieldValue = HelperFunctions::normalizeLWS(line.substr(colonPos + 1));
//     storeHeader(fieldName, fieldValue, headerMap);

//     return true;
// }



// std::map<std::string, std::vector<std::string> >& ParseHeader::getHeaderValues() const {
// 	return headerMap;
// }



#include "Header.hpp"

clsHeader::clsHeader() : _header_mask(0) {}

clsHeader::~clsHeader() {}

bool clsHeader::parseHeader(const std::string& line, HttpError& error) {
    size_t colonPos = line.find(':');
    
    if (colonPos == std::string::npos || colonPos == 0)
        return (error.setStatus(400, "Bad Request: Malformed Header"), false);

    if (colonPos > 0 && (line[colonPos - 1] == ' ' || line[colonPos - 1] == '\t'))
        return (error.setStatus(400, "Bad Request: Space before colon"), false);

    std::string key = _parseKey(line, colonPos);
    std::string value = _parseValue(line, colonPos);
    header_id::e_header_id id = _getHeaderID(key);

    if (id != -1) {
        if (_is_singleton(id)) {
            if (_header_mask & (1 << id))
                return (error.setStatus(400, "Bad Request: Duplicate Singleton Header"), false);
            _header_mask |= (1 << id);
        }
        _known_headers[id] = value;
    } else {
        _set_unknownHeader(key, value);
    }
    return true;
}


bool clsHeader::_is_singleton(header_id::e_header_id id) const {
    switch (id) {
        case header_id::H_HOST:
        case header_id::H_CONTENT_LENGTH:
        case header_id::H_CONTENT_TYPE:
        case header_id::H_EXPECT:
            return true;
        default:
            return false;
    }
}

std::string clsHeader::_parseKey(const std::string& line, size_t colonPos) const {
    std::string key = line.substr(0, colonPos);
    for (size_t i = 0; i < key.length(); ++i)
        key[i] = std::tolower(key[i]);
    return key;
}

std::string clsHeader::_parseValue(const std::string& line, size_t colonPos) const {
    std::string value = line.substr(colonPos + 1);
    size_t first = value.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = value.find_last_not_of(" \t");
    return value.substr(first, (last - first + 1));
}

void clsHeader::_set_unknownHeader(const std::string& key, const std::string& value)
{
    _unknownHeaders[key] = value;
}

const std::string& clsHeader::getHeader(header_id::e_header_id id) const
{
    return _known_headers[id];
}

header_id::e_header_id clsHeader::_getHeaderID(const std::string& key) const {
    size_t len = key.length();

    switch (len) {
        case 4:
            if (key == "host") return header_id::H_HOST;
            break;
        case 6:
            if (key[0] == 'e' && key == "expect") return header_id::header_id::H_EXPECT;
            if (key[0] == 'c' && key == "cookie") return header_id::header_id::H_COOKIE;
            break;
        case 10:
            if (key == "connection") return header_id::H_CONNECTION;
            break;
        case 12:
            if (key[0] == 'c' && key == "content-type") return header_id::H_CONTENT_TYPE;
            if (key[0] == 'u' && key == "user-agent") return header_id::H_USER_AGENT;
            break;
        case 14:
            if (key == "content-length") return header_id::H_CONTENT_LENGTH;
            break;
        case 17:
            if (key == "transfer-encoding") return header_id::H_TRANSFER_ENCODING;
            break;
    }
    return static_cast<header_id::e_header_id>(-1);
}
