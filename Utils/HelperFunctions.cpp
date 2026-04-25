#include "HelperFunctions.hpp"

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

long HelperFunctions::hexToDecS_view(const char *buf, int len)
{
	long decimalValue = 0;
	
	for (int i = 0; i < len; ++i)
	{
		int digit = hexCharToDec(buf[i]);
		
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

s_view HelperFunctions::find_last_of_view(s_view view, const char* set) {
	s_view result;

	if (!view.Data || view.len == 0 || !set || *set == '\0')
		return result;

	for (int i = (int)view.len - 1; i >= 0; --i) {
		char current = view.Data[i];
		
		for (const char* s = set; *s != '\0'; ++s) {
			if (current == *s) {
				result.Data = view.Data + i;
				result.len = view.len - i;
				return result;
			}
		}
	}
	return result;
}

s_view  HelperFunctions::find_first_of_view(s_view view, const char* set)
{

	s_view result;

	if (!view.Data || view.len == 0 || !set || *set == '\0')
		return result;

	for (size_t i = 0; i < view.len; ++i) {
		char current = view.Data[i];
		for (const char* s = set; *s != '\0'; ++s) {
			if (current == *s) {
				result.Data = view.Data + i;
				result.len = view.len - i;
				return result;
			}
		}
	}
	return result;
}

s_view	HelperFunctions::extract_between(s_view view, const char* start_set, const char* end_set)
{
	s_view result;

	s_view s_found = HelperFunctions::find_first_of_view(view, start_set);
	if (!s_found.Data) return result;

	s_view e_found;
	e_found.Data = s_found.Data + 1;
	e_found.len = view.len - (e_found.Data - view.Data);
	
	e_found = HelperFunctions::find_first_of_view(e_found, end_set);
	if (!e_found.Data)
		return result;

	result.Data = s_found.Data + 1; 
	result.len = e_found.Data - result.Data;

	return result;
}




// Achraf

std::map<int, std::string> HelperFunctions::_Message; 
std::map<std::string, std::string> HelperFunctions::_TypeContent;
std::map<int, std::string> HelperFunctions::_Body;
char HelperFunctions::_PoinerType[10] = {0};
char  **HelperFunctions::_ENV_VAR_CONST;
bool HelperFunctions::_Flag = false;

void HelperFunctions::StoreVarConst()
{
	_ENV_VAR_CONST = new(std::nothrow) char*[10];
	if (_ENV_VAR_CONST)
		HelperFunctions::ft_memset(_ENV_VAR_CONST, 0,(sizeof(_ENV_VAR_CONST) * 10));
	if (_ENV_VAR_CONST && !_Flag)
	{
		_Flag = true;
		_ENV_VAR_CONST[0] = ft_strdup("SERVER_SOFTWARE=FastHTTP/1.1");
		_ENV_VAR_CONST[1] = ft_strdup("SERVER_NAME=FastServer");
		_ENV_VAR_CONST[2] = ft_strdup("SERVER_PROTOCOL=HTTP/1.1");
		_ENV_VAR_CONST[3] = ft_strdup("GATEWAY_INTERFACE=CGI/1.1");
		_ENV_VAR_CONST[4] = ft_strdup("REMOTE_IDENT=\"\"");
		_ENV_VAR_CONST[5] = ft_strdup("REMOTE_HOST=\"\"");
	}
}

char *HelperFunctions::GetENV_VAR_CONST(short Index)
{
	if (!_ENV_VAR_CONST || Index > 8)
		return NULL;
	return _ENV_VAR_CONST[Index];
}

bool HelperFunctions::CmpWord(const char *Str, const std::string &Word, short SizeStr) {
    short i = 0;

    while (i < SizeStr && i < (short)Word.length())
    {
        if (std::tolower(Str[i]) != Word[i])
            return false;
        i++;
    }
    return (true);
}

bool HelperFunctions::IsStringDigit(const std::string &StringDigit, short Start, short End)
{
	if (StringDigit.empty())
		return false;
	for (short i = Start; i < (short)StringDigit.size() && i < End; i++)
	{
		if (!std::isdigit(StringDigit[i]))
			return false;
	}
	return true;
}

bool HelperFunctions::Iswhaitspace(char C) {
	return (C == ' ' || C == '\t');
}

void HelperFunctions::ConvertStringToLower(std::string &Str, short Size)
{
	for (short i = 0; i < (short)Str.size() && i < Size; i++)
		if (std::isalpha(Str[i]))
			Str[i] = std::tolower(Str[i]);
}
std::string HelperFunctions::ConvertStringToUpper(std::string &Str) {
    for (size_t i = 0; i < Str.size(); i++) {
        if (std::isalpha(Str[i]))
            Str[i] = std::toupper(Str[i]);
    }
    return Str;
}

bool HelperFunctions::Ischar(const std::string &Sep, char C) {
	for (size_t i = 0; i < Sep.size(); i++)
	{
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

int HelperFunctions::SkeepAtLast(const std::string& Str, const std::string &Sep)
{
	int End = Str.length() - 1;
	while (End >= 0 && Ischar(Sep, Str[End]))
			End--;
	return End;
}

int HelperFunctions::SkeeSep(const std::string &Str, char Sep)
{
	size_t i = 0;
	while (i < Str.size() && Sep == Str[i])
		i++;
	return i;
}


int HelperFunctions::ReadData(int FD, std::string &Data, ssize_t Size)
{
	Data.resize(Size);
	ssize_t SizeByte = read(FD, &Data[0], Size);
	if (SizeByte < 0) return -1;
	if (SizeByte != Size) Data.resize(SizeByte);
	return SizeByte;
}

void HelperFunctions::GetCleanLineHeader(const char *BigData, std::string &CleanLine ,short &MaxSizeHeader, bool &Flag, short &i, short LengthData)
{
	while(i < LengthData && BigData[i] != '\n')
	{
		(MaxSizeHeader)++;
		if ((MaxSizeHeader) > 4000)
			return ;
		CleanLine += BigData[i];
		i++;
	}
	if (BigData[i] == '\n')
	{
		(MaxSizeHeader)++;
		CleanLine += BigData[i];
		(Flag) = true;
		i++;
	}
	
}

std::string HelperFunctions::GTMHTTP(tm* GMT)
{
	const std::string Days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	const std::string Months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	std::stringstream ss;
	ss << Days[GMT->tm_wday] << ", " << GMT->tm_mday << " " << Months[GMT->tm_mon] << " " 
	   << GMT->tm_year + 1900 << " " << GMT->tm_hour << ":" << GMT->tm_min << ":" << GMT->tm_sec << " GMT";
	return ss.str();
}

std::string HelperFunctions::DateTime()
{
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

unsigned long HelperFunctions::getCurrentTimeInMs()
{
    return getCurrentTimeInS() * 1000;
}

long HelperFunctions::getCurrentTimeInS()
{
	long Time;
	Time = time(0);
	return (Time);
}

size_t	HelperFunctions::ft_strlen(const char *s)
{
	size_t	i = 0;
 
	if (!s)
		return 0;
	while (s[i])
		i++;
	return i;
}

char	*HelperFunctions::ft_strdup(const char *src)
{
	int		l;
	int		i;
	char	*dest_dup;

	if (!src)
		return (NULL);
	i = 0;
	l = ft_strlen(src);
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

void HelperFunctions::NumToStr(int Number, std::string &Str)
{
    char Remainder = 0;
    int NewNumbr = 0;
    int Counter = 0;
	if (Number == 0)
	{
		Str += '0';
		return ;
	}
    if (Number < 0)
    {
        Number *= -1;
        Str += '-';
    }
    while (Number)
    {
        NewNumbr =( NewNumbr * 10) + Number % 10;
        Number /= 10;
        Counter++;
    }
    while (NewNumbr)
    {
        Remainder = (NewNumbr % 10) + '0';
        Str += Remainder;
        NewNumbr /= 10;
        Counter--;
    }
    while (Counter > 0)
    {
        Str += '0';
        Counter--;
    }
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
	int_char = new(std::nothrow) char[len + 1];
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

const char  *HelperFunctions::GetTypeDataFile(const std::string &Str)
{
    size_t Pos;
    short i = 0;
    if ((Pos = Str.find('.')) == std::string::npos)
    {
        _PoinerType[0] = '\0';
        return _PoinerType;
    }
    while (i < 10 && Pos < Str.length())
    {
        _PoinerType[i] = Str[Pos];
        Pos++;
        i++;
    }
    _PoinerType[i] = '\0';
    return (_PoinerType);
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
		_TypeContent[".js"	]   = "text/javascript";
		_TypeContent[".jpg"]  = "image/jpeg";
		_TypeContent[".jpeg"] = "image/jpeg";
		_TypeContent[".png"]  = "image/png";
		_TypeContent[".txt"]  = "text/plain";
		_TypeContent[".mp4"]  = "application/mp4";
	}
 }

const char *HelperFunctions::GetType(const std::string &Type)
{
	if (_TypeContent.count(Type))
		return  _TypeContent[Type].c_str();
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
	_Body[502] = "Body502";
 
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
	_Message[502] = "Bad Gateway";
 }

 const char *HelperFunctions::GetStatusMessage(int Status) 
{
	if (_Message.count(Status))
		return  _Message[Status].c_str();
	return ("Unknown Status");
}

const char *HelperFunctions::GetBody(int Status)
{
	if (_Body.count(Status))
		return  _Body[Status].c_str();
	return ("Unknown Body");
}


int HelperFunctions::Countword(const std::string &Str, const std::string &Sep)
{
	int i = 0;
	int counter = 0;
	bool Flag = false;
	while (i < (int)Str.length())
	{
		if (Ischar(Sep, Str[i]))
		{
			if (Flag)
				counter++;
			Flag = false;
		}
		else
			Flag = true;
		i++;
	}
	if (Flag)
		counter++;
	return counter;
}

void HelperFunctions::CopyStr(const std::string &Str_src, std::string &Str_new, short Start, short Pos)
{
	short i = Start;
	while(i < (short)Str_src.length() && i < Pos)
	{
		Str_new += Str_src[i];
		i++;
	}
}

void HelperFunctions::CopyStr(const char *Str_src, std::string &Str_new, short Start, short Length)
{
	short i = Start;
	while(i < Length)
	{
		Str_new += Str_src[i];
		i++;
	}
}

short HelperFunctions::LengthWord(const std::string &Str, const std::string &Sep, short Start)
{
    short i = Start;
    short count = 0;
    while(i < (short)Str.length())
    {
        if (Ischar(Sep, Str[i]))
            return (count);
        count++;
        i++;
    }
    return (count);
}

stEventProcess::eEventProcess HelperFunctions::checkProcessStatus(int pid)
{
	int status;
	int exit_code = waitpid(pid, &status, WNOHANG);

	if (exit_code != 0)
	{
        if (WIFEXITED(status)) // if exited
        {
            if (WEXITSTATUS(status) != 0) // check status
    			return stEventProcess::END_UNKNOW;
            else
                return stEventProcess::THE_END;
        }
        else if (WIFSIGNALED(status) || exit_code == -1)
    		return stEventProcess::END_UNKNOW;
		else
			return stEventProcess::THE_END;
	}
	return stEventProcess::RUNINNG;
}
