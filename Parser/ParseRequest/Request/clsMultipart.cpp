#include "clsMultipart.hpp"

// multi header funtions
std::set<char> clsMultiHeader::forbidden;

clsMultiHeader::clsMultiHeader()
{
    if (forbidden.size() == 0)
    {
        forbidden.insert('(');
        forbidden.insert(')');
        forbidden.insert('<');
        forbidden.insert('>');
        forbidden.insert('@');
        forbidden.insert(',');
        forbidden.insert(';');
        forbidden.insert(':');
        forbidden.insert('\\');
        forbidden.insert('"');
        forbidden.insert('/');
        forbidden.insert('[');
        forbidden.insert(']');
        forbidden.insert('?');
        forbidden.insert('=');
        forbidden.insert('{');
        forbidden.insert('}');
    }
}
    

void clsMultiHeader::moveOffsetToDel(uint16_t &trav)
{
	while (trav < ofset)
	{
		if (Header[trav] == ';')
			return;
		else if (trav + 1 < ofset && Header[trav] == '\r' && Header[trav + 1] == '\n')
			return;
		trav++;
	}
}

void clsMultiHeader::skipWhiteSpaces(const char *str, uint16_t &trav, uint16_t size)
{
	while (trav < size && (str[trav] == ' ' || str[trav] == '\t'))
		trav++;
}

// bool StoreValueAfterDelim(string &arr, uint16_t &trav, uint16_t &cur)
// {
//     short quotes = 0;
//     bool backSlach = false;

//     skipWhiteSpaces(Header.c_str(), trav, Header.size());
//     while (trav < Header.size() && Header[trav] != '\r')
//     {
//         if ((arr.length() != 0 && quotes != 1 && forbidden.count(Header[trav])) || (Header[trav] >= 0 && Header[trav] <= 32) || Header[trav] == 127)
//         {
//             if (Header[trav] == ';')
//             {
//                 trav++;
//                 break;
//             }
//             else if (Header[trav] == ' ' || Header[trav] == '\t')
//                 skipWhiteSpaces(Header.c_str(), trav, Header.size());
//             else
//                 return false;
//         }
//         else
//         {
//             if (backSlach == false && Header[trav] == '"')
//             {
//                 if (arr.length() == 0 || quotes == 1)
//                     quotes++;
//                 else
//                     return false;
//                 trav++;
//             }
//             else
//             {
//                 if (backSlach == false && Header[trav] == '\\')
//                     backSlach = true;
//                 else if (quotes != 2)
//                 {
//                     arr += Header[trav++];
//                     backSlach = false;
//                 }
//                 else
//                     return false;
//             }
//         }
//     }
//     if (quotes == 1)
//         return 1;
//     cur = trav;
//     return true;
// }

char clsMultiHeader::sanitizeKey(char *arr, uint16_t start, uint16_t end, char del, int start_end[2])
{
	start_end[0] = start;
	start_end[1] = -1;

	if (start == end)
		return 'E';
	while (start < end)
	{
		if (arr[start] == del)
		{
			start_end[1] = start;
			return 'S';
		}
		else if (forbidden.count(arr[start]) || (arr[start] >= 0 && arr[start] <= 32) || arr[start] == 127)
			return 'E';
		arr[start] = std::tolower(arr[start]);
		start++;
	}
	return 'E';
}

char clsMultiHeader::sanitizeVal(char *arr, uint16_t start, uint16_t end, int start_end[2])
{
	skipWhiteSpaces(Header, start, ofset);
	if (start == end)
		return 'E';

	start_end[0] = start;
	start_end[1] = -1;

	bool openQuotes = false;
	bool closedQuotes = false;
	bool backSlach = false;
	bool getData = false;
	bool done = false;
	while (start < end)
	{
		if (arr[start] == '\\' && (openQuotes == true && closedQuotes == false))
		{
			backSlach = true;
			start++;
		}
		else if (backSlach == false && arr[start] == '\"' && (openQuotes == false || closedQuotes == false))
		{
			if (getData == true && openQuotes == false)
				return 'E';
			else if (openQuotes == false)
			{
				start_end[0] = start + 1;
				openQuotes = true;
			}
			else if (closedQuotes == false)
			{
				start_end[1] = start;
				closedQuotes = true;
			}
			else
				return 'E';
			start++;
		}
		else
		{
			if ((arr[start] >= 0 && arr[start] <= 31) || arr[start] == 127)
				return 'E';
			else if ((closedQuotes == true || openQuotes == false) && forbidden.count(arr[start]))
				return 'E';
			else if ((closedQuotes == true || done == true) && arr[start] != ' ' && arr[start] != '\t')
			{
				return 'E';
			}
			else if ((closedQuotes == true || openQuotes == false) && (arr[start] == ' ' || arr[start] == '\t'))
			{
				if (done == false)
				{
					start_end[1] = start;
					done = true;
				}
				skipWhiteSpaces(Header, start, ofset); // test this things and add algo taking key and value
			}
			else
			{
				start++;
				getData = true;
			}
		}
	}
	if (openQuotes == true && closedQuotes == false)
		return 'E';
	if (done == false)
	{
		start_end[1] = start;
		done = true;
	}
	return 'S';
}

char clsMultiHeader::myComparaison(int st_end_key[2], bool parms)
{
	int size = st_end_key[1] - st_end_key[0];
	int start = st_end_key[0];

	if (size == 19 && strncmp(&Header[start], "content-disposition", 19) == 0)
		return CONTENT_DISPO;
	else if (size == 12 && strncmp(&Header[start], "content-type", 19) == 0)
		return CONTENT_TYPE;
	else if (parms)
	{
		if (size == 4 && strncmp(&Header[start], "name", 4) == 0)
			return NAME_PAR;
		else if (size == 8 && strncmp(&Header[start], "filename", 4) == 0)
			return FILENAME_PAR;
	}
	return 0;
}

void clsMultiHeader::storeValue(int value_indexes[2], string &value)
{
	int size = value_indexes[1] - value_indexes[0];
	int start = value_indexes[0];

	if (size <= 0)
	{
		value = "";
		return;
	}

	value.resize(size);
	std::memcpy(&value[0], &Header[start], size);
}

char clsMultiHeader::sanitizeKeyAndValue(int st_end_key[2], int st_end_value[2], uint16_t start, uint16_t end, uint16_t temp)
{
    if (temp)
    {
        skipWhiteSpaces(Header, start, ofset);
        if (sanitizeKey(Header, start, end, '=', st_end_key) == 'E')
            return 'E';
    }
    else
    {
        if (sanitizeKey(Header, start, end, ':', st_end_key) == 'E')
            return 'E';
    }
    return sanitizeVal(Header, st_end_key[1] + 1, end, st_end_value);
}

char clsMultiHeader::storeValues(int key_indexes[2], int value_indexes[2], short delTot)
{
	bool isParms = delTot > 0;

	if (isParms && insideDisposition)
	{
		char whichPams = myComparaison(key_indexes, isParms);
		if (whichPams == NAME_PAR)
		{
			if (this->nameVarEx)
				return 'E';
			this->nameVarEx = true;
		}
		else if (whichPams == FILENAME_PAR)
		{
			if (fileNameEx)
				return 'E';
			fileNameEx = true;
			storeValue(value_indexes, fileNameValue);
		}
	}
	else if (isParms == false)
	{
		insideDisposition = false; // reset it every time we check header key

		char whichPams = myComparaison(key_indexes);
		if (whichPams == CONTENT_TYPE)
		{
			if (contentTypeEx)
				return 'E';
			contentTypeEx = true;
			storeValue(value_indexes, contentTypeValue);
		}
		else if (whichPams == CONTENT_DISPO)
		{
			if (contentDispoEx)
				return 'E';
			contentDispoEx = true;
			insideDisposition = true;
			storeValue(value_indexes, contentDispoValue);
			if (contentDispoValue != "form-data")
				return 'E';
		}
	}
	return 'S';
}

void clsMultiHeader::addChar(char c)
{
	if (ofset == 4000)
	{
		isError = true;
		return;
	}
	Header[ofset++] = c;
};
bool clsMultiHeader::moveOffsetToDel(int &cur, int &trav)
{
	while (trav < this->ofset)
	{
		if (Header[trav] == ':')
			return true;
		else if (Header[trav] == '\n')
			cur = trav + 1;
		trav++;
	}
	return false;
}

void clsMultiHeader::Parsing()
{
    uint16_t cur = 0;
    uint16_t trav = 0;
    short delTot = 0;

	int key_indexes[2];
	int value_indexes[2];

	while (trav < ofset)
	{
		moveOffsetToDel(trav);

		if ((sanitizeKeyAndValue(key_indexes, value_indexes, cur, trav, delTot) == 'E') || (storeValues(key_indexes, value_indexes, delTot) == 'E'))
		{
			this->isError = true;
			return ;
		}
		if (Header[trav] == ';')
		{
			++delTot;
			++trav; // skip delimeter
			cur = trav;
		}
		else
		{
			delTot = 0; // reset total delimiters in line
			trav += 2;  // skip crlf
			cur = trav;
		}
	}
	if (this->nameVarEx == false)
		this->isError = true;
};

string clsMultiHeader::getFileName()
{
	return fileNameValue;
};
string clsMultiHeader::getContentType()
{
	return contentTypeValue;
};
bool clsMultiHeader::getError()
{
	return isError;
};
void clsMultiHeader::Reset()
{
	ofset = 0;
	isError = false;
};

// multipart functions

void clsMultiPart::InitializeMulti(char *boundary, short lenBound, uint16_t start)
{
    this->lenBound = lenBound;
    std::memcpy(this->boundary, boundary, lenBound);
    std::memcpy(endNormal, "\r\n", 2);
    std::memcpy(endFinal, "--", 2);
    _hitEnd = false;
    foundFirstBound = false;
    cur = start;
    trav = cur;
    processIn = HEADER;
    error = false;
    multiHeaders.Reset();
}

whichBound clsMultiPart::isBoundary(char *arr, bool edgeCase)
{
	if (foundFirstBound == false || edgeCase == true)
	{
		if (strncmp(arr, boundary, lenBound) == 0)
		{
			arr += lenBound;
			if (strncmp(arr, "\r\n", 2) == 0)
			{
				foundFirstBound = true;
				return startBoundary;
			}
			else if (strncmp(arr, endFinal, 2) == 0)
				return endBoundary;
			else
				return None;
		}
	}
	else if (strncmp(arr, "\r\n", 2) == 0)
	{
		arr += 2;
		if (strncmp(arr, boundary, lenBound) == 0)
		{
			arr += lenBound;
			if (strncmp(arr, endFinal, 2) == 0)
				return endBoundary;
			else if (strncmp(arr, endNormal, 2) == 0)
				return midBoundary;
			else
				return None;
		}
	}
	return None;
};

void clsMultiPart::Parser(char *arr, uint16_t &offset)
{
    while (trav < offset && cur < offset && error == false)
    {
        if (_hitEnd)
            trav++;
        else if (foundFirstBound && arr[trav] != '\r')
        {
            if (processIn == HEADER)
            {
                multiHeaders.addChar(arr[trav]);
                if (multiHeaders.getError() == true)
                {
                    error = true;
                    return;
                }
            }
            // else if (processIn == BODY);
            // write into the right file;
            // to be continue
            // i must link headers with real path and so forth
            trav++;
        }
        else
        {
            if (offset - trav >= lenBound + 2)
            {
                static bool edgeCase = false;
                if (processIn == HEADER && strncmp("\r\n\r\n", &arr[trav], 4) == 0)
                {
                    edgeCase = true;
                    processIn = END_HEADER;
                    trav += 4;
                }
                if ((edgeCase == false && offset - trav < lenBound + 4) || (edgeCase && offset - trav < lenBound + 2))
                    return;
                whichBound boundType = isBoundary(&arr[trav], edgeCase);
                if (boundType != None)
                {
                    // if (processIn == BODY)
                    //     ;
                    // reset body data
                    if (boundType == endBoundary)
                        this->_hitEnd = true;
                    else if (boundType == startBoundary)
                    {
                        edgeCase = true;
                        foundFirstBound = true;
                    }
                    processIn = HEADER;
                    trav += lenBound + 2 + !edgeCase * 2;
                }
                else if (edgeCase == false)
                {
                    if (processIn == HEADER)
                        multiHeaders.addChar(arr[trav]);
                    // else
                    //     ; // add body here
                    // handleBody
                    trav += 1;
                }
                edgeCase = false;
            }
            else
            {
                return;
            }
            if (processIn == END_HEADER)
            {
                multiHeaders.Parsing();
                if (multiHeaders.getError())
                {
                    error = true;
                    return;
                }
                processIn = BODY;
            }
        }
    }
}
bool clsMultiPart::getError()
{
	return this->error;
}