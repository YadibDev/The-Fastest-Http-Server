#include "HelperFunctions.hpp"

std::string HelperFunctions::trim(const std::string& str) {
	const std::string whitespace = " \t";
	
	size_t first = str.find_first_not_of(whitespace);
	
	if (first == std::string::npos) {
		return "";
	}
	
	size_t last = str.find_last_not_of(whitespace);
	
	return str.substr(first, (last - first + 1));
}

void HelperFunctions::skipWhitespace(const std::string& str, size_t &pos) {
	while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\t'))
		pos++;
}

bool HelperFunctions::isCRLF(const std::string& str) {
	return (str == "\r\n");
}

int hexCharToDec(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return -1;
}

long HelperFunctions::hexToDec(const std::string& hex)
{
	long decimalValue = 0;
	
	for (std::size_t i = 0; i < hex.length(); ++i)
	{
		int digit = hexCharToDec(hex[i]);
		
		if (digit == -1)
			return -1;
		
		decimalValue = (decimalValue << 4) | digit;
	}
	
	return decimalValue;
}

bool HelperFunctions::is_numeric(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++)
		if (!std::isdigit(str[i])) return false;
	return true;
}


bool HelperFunctions::is_CTLsString(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++)
		if (std::iscntrl(str[i])) return true;
	return false;
}

bool    HelperFunctions::checkIfTheFirstWord(std::string str, std::string Start, size_t POS)
{

	for (size_t i = POS; i < str.size(); i++)
	{
		if (str[i] == ' ' || str[i] == '\t')
			break ;
		if (str[i] != Start[i])
			return (false);
	}
	return (true);
}

bool HelperFunctions::isLWS(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::string HelperFunctions::normalizeLWS(const std::string& input)
{
    std::string result;
    size_t i = 0;
    bool inSpace = false;

    while (i < input.size() && isLWS(input[i]))
        i++;

    for (; i < input.size(); i++)
    {
        if (isLWS(input[i]))
            inSpace = true;
        else
        {
            if (inSpace && !result.empty())
                result += ' ';
            result += input[i];
            inSpace = false;
        }
    }
    return result;
}

bool HelperFunctions::myIsspace(std::string str, size_t pos)
{
	for (size_t i = pos; i < str.size(); i++)
	{
		if (isLWS(str[i]))
			return (true);
	}
	return (false);
}

std::vector<std::string> HelperFunctions::splitCommaSeparated(const std::string& value)
{
    std::vector<std::string> result;
    std::string current;

    for (size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == ',')
        {
            current = normalizeLWS(current);
            if (!current.empty())
                result.push_back(current);
            current.clear();
        }
        else
            current += value[i];
    }

    current = normalizeLWS(current);
    if (!current.empty())
        result.push_back(current);

    return result;
}

bool HelperFunctions::strIsSpace(const std::string &str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (!isspace(str[i]))
            return false;
    }
    return true;
}

bool HelperFunctions::isBoundary(const std::string &str, const std::string &boundary, std::string &remander)
{
    std::string combined = remander + str;
    if (combined.size() < boundary.size())
    {
        remander = combined;
        return false;
    }
    size_t pos = combined.find(boundary);
    if (pos != std::string::npos) {
        remander = combined.substr(pos + boundary.size());
        return true;
    }
    if (combined.size() >= boundary.size())
        remander = combined.substr(combined.size() - boundary.size() - 1);
    else
        remander = combined;
    return false;
}

short HelperFunctions::isValidPath(const std::string& path, bool expectDir)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return (403);

	if (expectDir && !S_ISDIR(info.st_mode))
		return (403);

	if (!expectDir && S_ISDIR(info.st_mode))
		return (403);

	return (200);
}

// Achraf

bool HelperFunctions::CmpWord(const std::string &BigStr, const std::string &Word, bool Switch) {
    int i = 0;
    int lenghtBigStr = BigStr.size();
    int lenghtWord = Word.size();

    if (Switch) {
        while (i < lenghtBigStr && i < lenghtWord) {
            if (BigStr[i] != Word[i])
                return false;
            i++;
        }
    } else {
        while (lenghtBigStr > 0 && lenghtWord > 0) {
            if (BigStr[--lenghtBigStr] != Word[--lenghtWord])
                return false;
        }
    }
    return (!lenghtWord || (Switch && lenghtWord == i));
}

size_t HelperFunctions::FindCRLF(const std::string &Str, const std::string &CRLF) {
    size_t i = (Str.size()) ? Str.size() - 1 : 0;
    size_t j = (CRLF.size()) ? CRLF.size() - 1 : 0;
    if (Str.empty())
        return std::string::npos;
    while (true) {
        if (!Iswhaitspace(Str[i])) {
            if (CRLF[j] != Str[i])
                break;
        }
        if (i == 0 || j == 0)
            break;
        if (!Iswhaitspace(Str[i]))
            j--;
        i--;
    }
    return (j == 0) ? i : std::string::npos;
}

bool HelperFunctions::IsStringDigit(const std::string &StringDigit) {
    if (StringDigit.empty()) return false;
    for (size_t i = 0; i < StringDigit.size(); i++) {
        if (!std::isdigit(StringDigit[i]))
            return false;
    }
    return true;
}

bool HelperFunctions::Iswhaitspace(char C) {
    return (C == ' ' || C == '\t');
}

std::string HelperFunctions::TrimStr(std::string Str, const std::string &Sep) {
    if (Str.empty()) return Str;
    size_t Start = 0;
    size_t End = Str.length() - 1;
    while (Start < Str.length() && Ischar(Sep, Str[Start]))
        Start++;
    while (End > Start && Ischar(Sep, Str[End]))
        End--;
    return Str.substr(Start, End - Start + 1);
}

std::string HelperFunctions::ConvertStringToLower(std::string &Str)
{
    for (size_t i = 0; i < Str.size(); i++) {
        if (std::isalpha(Str[i]))
            Str[i] = std::tolower(Str[i]);
    }
    return Str;
}
std::string HelperFunctions::ConvertStringToUpper(std::string &Str) {
    for (size_t i = 0; i < Str.size(); i++) {
        if (std::isalpha(Str[i]))
            Str[i] = std::toupper(Str[i]);
    }
    return Str;
}

bool HelperFunctions::Ischar(const std::string &Sep, char C) {
    for (size_t i = 0; i < Sep.size(); i++) {
        if (Sep[i] == C)
            return true;
    }
    return false;
}

int HelperFunctions::SkeeSep(const std::string &Str, const std::string &Sep) 
{
    int i = 0;
    while (i < (int)Str.size() && Ischar(Sep, Str[i]))
        i++;
    return i;
}

int HelperFunctions::SkeeSep(const std::string &Str, char Sep)
{
    size_t i = 0;
    while (i < Str.size() && Sep == Str[i])
        i++;
    return i;
}

void  HelperFunctions::Split(std::vector<std::string> &Strings, std::string Str, char Sep, int TimesSplit)
{
    unsigned long Pos = 0;
    std::string  SepString = "";
    const char *PtrStr = NULL;
    int i = 0;
   
    if ((Pos = Str.find(Sep)) == std::string::npos)
        Strings.push_back(Str);
    else
    {
        Str = Str.substr(SkeeSep(Str, Sep));
        while ((Pos = Str.find(Sep)) != std::string::npos)
        {
            if (i == TimesSplit - 1 && TimesSplit > 0)
                break;
            else if (TimesSplit > 0)
                i++;
            Strings.push_back(Str.substr(0,Pos));
            Pos += SkeeSep(Str.substr(Pos),Sep);
            Str = Str.substr(Pos);
        }
        if (!Str.empty())
            Strings.push_back(Str);
    }
}

int HelperFunctions::ReadData(int FD, std::string &Data, ssize_t Size) {
    Data.resize(Size);
    ssize_t SizeByte = read(FD, &Data[0], Size);
    if (SizeByte < 0) return -1;
    if (SizeByte != Size) Data.resize(SizeByte);
    return SizeByte;
}

std::string HelperFunctions::GetNextLine(int FD, std::string &BigData, ssize_t Size)
{
    std::string Buffer;
    std::string CleanLine;
    size_t Pos = 0;
    ssize_t SizeByte = 0;

    while ((Pos = BigData.find('\n')) == std::string::npos)
    {
        SizeByte = ReadData(FD, Buffer, Size);
        if (SizeByte < 0)
            return "";
        if (SizeByte == 0)
            break;
        BigData += Buffer;
    }
    if (Pos != std::string::npos)
    {
        Pos += 1;
        CleanLine = BigData.substr(0, Pos);
        BigData = BigData.substr(Pos);
    }
    else
    {
        CleanLine = BigData;
        BigData.clear();
    }
    return CleanLine;
}

void HelperFunctions::GetCleanLine(std::string &BigData, std::string &CleanLine)
{
    size_t Pos = 0;
    Pos = BigData.find('\n');
    if (Pos != std::string::npos)
    {
        Pos += 1;
        CleanLine = BigData.substr(0, Pos);
        BigData = BigData.substr(Pos);
    }
    else
    {
        CleanLine = BigData;
        BigData.clear();
    }
}

std::string HelperFunctions::GTMHTTP(tm* GMT) {
    const std::string Days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const std::string Months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    std::stringstream ss;
    ss << Days[GMT->tm_wday] << ", " << GMT->tm_mday << " " << Months[GMT->tm_mon] << " " 
       << GMT->tm_year + 1900 << " " << GMT->tm_hour << ":" << GMT->tm_min << ":" << GMT->tm_sec << " GMT";
    return ss.str();
}

std::string HelperFunctions::DateTime() {
    time_t Time = time(0);
    tm* GMT = gmtime(&Time);
    return GTMHTTP(GMT);
}

std::string HelperFunctions::Convert_Hex(const std::string &Str, int Num) {
    int		i = 0;
	std::string	MaxHex;
	std::string	Result;

	if (Num == 0)
		return ("0");
	while (Num > 0)
	{
		MaxHex += Str[Num % 16];
		Num /= 16;
	}
    i = MaxHex.size() - 1;
	while (i >= 0)
		Result += MaxHex[i--];
	return (Result);
}

unsigned long long HelperFunctions::getCurrentTimeInS()
{
    long Time;
    Time = time(0);
    return (Time);
}
size_t	HelperFunctions::_ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
		i++;
	return (i);
}

char	*HelperFunctions::ft_strdup(const char *src)
{
	int		l;
	int		i;
	char	*dest_dup;

	if (!src)
		return (NULL);
	i = 0;
	l = _ft_strlen(src);
	dest_dup = new(std::nothrow) char[(l + 1)];
	if (!dest_dup)
		return (NULL);
	while (src[i])
	{
		dest_dup[i] = src[i];
		i++;
	}
	dest_dup[i] = '\0';
	return (dest_dup);
}

void	HelperFunctions::free_matrex(char ***matrex)
{
	int	i;

	i = 0;
	if (!*matrex)
		return ;
	while ((*matrex)[i])
	{
		delete[] ((*matrex)[i]);
		i++;
	}
	delete[] (*matrex);
	*matrex = NULL;
}

int	HelperFunctions::len_int(int nb)
{
	long	num;
	int		i;

	i = 0;
	num = nb;
	if (num == 0)
		return (1);
	if (num < 0)
	{
		i++;
	}
	while (num)
	{
		num = (num / 10);
		i++;
	}
	return (i);
}

char	*HelperFunctions::ft_itoa_negative(int n, char *int_char)
{
	long	num;
	int		len;

	len = len_int(n);
	num = n;
	if (num < 0)
	{
		int_char[0] = '-';
		num *= -1;
	}
	while (len > 1)
	{
		int_char[len - 1] = ((num % 10) + '0');
		num = (num / 10);
		len--;
	}
	int_char[len_int(n)] = '\0';
	return ((int_char));
}

char	*HelperFunctions::ft_itoa(int n)
{
	long	num;
	char	*int_char;
	int		len;
	int		prev_len;

	len = len_int(n);
    prev_len = len;
	num = n;
	int_char = new(std::nothrow) char(len + 1);
	if (!int_char)
		return (NULL);
	if (num < 0)
		return (ft_itoa_negative(n, int_char));
	while (len)
	{
		int_char[len - 1] = ((num % 10) + '0');
		num = (num / 10);
		len--;
	}
	int_char[prev_len] = '\0';
	return ((int_char));
}

std::string HelperFunctions::GetTypeDataFile(const std::string &Str)
{
    size_t Pos;
    if ((Pos = Str.find('.')) == std::string::npos)
        return "";
    return (Str.substr(Pos, Str.size()));
}

void	*HelperFunctions::ft_memset(void *str, int c, size_t n)
{
	char	*k;
	char	c1;

	k = (char *)str;
	c1 = (char)c;
	while (n--)
	{
		*k = c1;
		k++;
	}
	return (str);
}

void HelperFunctions::StoredDefaultType()
{
    if (_TypeContent.empty())
    {
        _TypeContent[".html"] = "text/html";
        _TypeContent[".htm"]  = "text/html";
        _TypeContent[".css"]  = "text/css";
        _TypeContent[".js"]   = "text/javascript";
        _TypeContent[".jpg"]  = "image/jpeg";
        _TypeContent[".jpeg"] = "image/jpeg";
        _TypeContent[".png"]  = "image/png";
        _TypeContent[".txt"]  = "text/plain";
    }
 }

const std::string &HelperFunctions::GetType(const std::string &Type)
{
    if (_TypeContent.count(Type))
        return  _TypeContent[Type];
    return "application/octet-stream";
}

void HelperFunctions::StoredBodys()
 {
    _Body[200] = "Body200";
    _Body[201] = "Body201";
    _Body[204] = "Body204";
    _Body[301] = "Body301";
    _Body[302] = "Body302";
    _Body[400] = "Body400";
    _Body[403] = "Body403";
    _Body[404] = "Body404";
    _Body[500] = "Body500";
    _Body[501] = "Body501";
 
 }
 void HelperFunctions::StoredMessage()
 {
    _Message[200] = "OK";
    _Message[201] = "Created";
    _Message[204] = "No Content";
    _Message[301] = "Moved Permanently";
    _Message[302] = "found";
    _Message[400] = "Bad Request";
    _Message[403] = "Forbidden";
    _Message[404] = "Not Found";
    _Message[500] = "Internal Server Error";
    _Message[501] = "Not Implemented";
 }

 const std::string &HelperFunctions::GetStatusMessage(int Status) 
{
    if (_Message.count(Status))
        return  _Message[Status];
    return ("Unknown Status");
}

const std::string &HelperFunctions::GetBody(int Status)
{
    if (_Body.count(Status))
        return  _Body[Status];
    return ("Unknown Status");
}

void HelperFunctions::JoinBuffer(char *OldStr, const char *Newstr, int *UpdateLength)
{
    int i = 0;

    while (Newstr[i])
    {
        OldStr[*UpdateLength] = Newstr[i];
        i++;
        (*UpdateLength)++;
    }
    OldStr[(*UpdateLength)] = '\0';
}