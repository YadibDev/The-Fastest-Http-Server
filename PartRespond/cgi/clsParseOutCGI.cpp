#include "clsParseOutCGI.hpp"

clsParseOutCGI::clsParseOutCGI(RequestHandler &DataRequest,std::string &Body, std::string &HeadersFieldFinal, std::string &FileNameFromDisk, std::string &InternalRedirectSrc)
	 :_DataRequest(DataRequest),_Body(Body), _HeadersFieldFinal(HeadersFieldFinal), _FileNameFromDisk(FileNameFromDisk), _InternalRedirectSrc(InternalRedirectSrc)
{
	_BytesBody = 0;
	_FoundBody = false;
	_IsConnectoin = true;
	_ProcessIsFinish = false;
	_CountSizeHeaders = 0;
	_Fdout = -1;
	_ModTransferData = false;
	_OffsetBody = 0;
	_HeadersFieldDuplicate.resize(MAX_HEADERS);
	_Line.resize(MAX_HEADERS);
	_NameHeader.resize(MAX_HEADERS);
	_ValueHeader.resize(MAX_HEADERS);

	if (_HeadersFieldDuplicate.empty() || _Line.empty()
		|| _NameHeader.empty() || _ValueHeader.empty())
	{
		_Mod[stMod::MEMORY_FAILD] = stMod::MEMORY_FAILD;
		return ;
	}
	_Status = 0;
	_NameHeader = "";
	_ValueHeader = "";
	_HeadersFieldDuplicate = "";
	_Line = "";

	HelperFunctions::ft_memset(_ExistHeaders, stHeadersCGI::EMPTY, sizeof(_ExistHeaders));
	HelperFunctions::ft_memset(_Mod, stMod::EMPTY, sizeof(_Mod));
}

bool clsParseOutCGI::_IsSpecialChar(char C)
{
	return (C == '!'|| C == '#' || C == '$' || C == '%' || C == '&'
			|| C == '\'' || C == '*' || C == '+' || C == '-' || C == '.'
			|| C == '^' || C == '_' || C == '|' || C == '~' || C == '`');
}

bool clsParseOutCGI::_IsValidHeaderValueChar(unsigned char C)
{
	return (C == '\t' || (C >= ' ' && C <= 126) || (C >= 128));
}

bool clsParseOutCGI::_CheckValidNameHeader(std::string &HeaderName, short Start, short End)
{
	if (HeaderName.empty())
		return (false);
	short i = Start;
	while (i < (short)HeaderName.length() && i < End)
	{
		if (!isalpha(HeaderName[i]) && !_IsSpecialChar(HeaderName[i])
			&& !isdigit(HeaderName[i]))
			return (false);
		i++;
	}
	return (true);
}

bool clsParseOutCGI::_CheckValidValueHeader(std::string &HeaderValue, short Satrt, short End)
{
	short i = Satrt;
	while (i < (short)HeaderValue.length() && i < End)
	{
		if (!_IsValidHeaderValueChar(HeaderValue[i]))
			return (false);
		i++;
	}
	return (true);
}

bool clsParseOutCGI::_LocationIsClientOrLocal(std::string &Location)
{
	if (Location.find("://") != std::string::npos)
		return (true);
	return (false);
}

short clsParseOutCGI::_AtoiStatusCode(const std::string &StringDigit, short Start, short End)
{
	short Number = 0;
	uint8_t counter = 0;
	for (short i = Start; i < (short)StringDigit.size() && i < End; i++)
	{
		counter++;
		Number = (10 * Number) + (StringDigit[i] - '0');
		if (counter == 4)
		    return Number;
	}
	return Number;
}

bool clsParseOutCGI::_ParseStatus(const std::string &LineValue)
{
	short NumberStatus = 0;
	short Start = HelperFunctions::SkeeSep(LineValue, " \t");
	short LengthWord = HelperFunctions::LengthWord(LineValue, " \t",Start);
	short End = Start + LengthWord;

	int Counter = HelperFunctions::Countword(LineValue, " \t");
	if (!Counter)
		return false;
	if (!HelperFunctions::IsStringDigit(LineValue, Start, End))
		return (false);
	NumberStatus = _AtoiStatusCode(LineValue, Start, End);
	if (NumberStatus < 100 || NumberStatus > 599)
		return (false);
	if (NumberStatus >= 400 && NumberStatus != 404)
	    _IsConnectoin = false;
	return (true);
}

bool clsParseOutCGI::_StoredHeadersField(std::string &Str)
{
	int Skeep = HelperFunctions::SkeeSep(_ValueHeader, " \t");
	if (_HeadersField.count(_NameHeader))
	{        
		if (!_NameHeader.compare("location") || !_NameHeader.compare("status") || !_NameHeader.compare("content-type"))
			return false;
		else if (!_NameHeader.compare("set-cookie"))
		{
			_HeadersFieldDuplicate += Str;
			_HeadersFieldDuplicate += "\r\n";
		}
		else if (Skeep != (int)_ValueHeader.length() && !_ValueHeader.empty())
		{
			_HeadersField[_NameHeader] += ",";                
			_HeadersField[_NameHeader] += _ValueHeader;
		}
	}
	else if (Skeep != (int)_ValueHeader.length() && !_ValueHeader.empty())
		_HeadersField[_NameHeader] = _ValueHeader;
	_NameHeader = "";
	_ValueHeader= "";
	return true;
}

bool clsParseOutCGI::_ValidHeaders(std::string &Str)
{
	size_t Pos = 0;
	if ((Pos = Str.find("\r\n")) != std::string::npos)
		Str.erase(Pos);
	else
		return (false);
	if ((Pos = Str.find(':')) == std::string::npos)
		return (false);
	if (!_CheckValidNameHeader(Str, 0, Pos))
		return (false);
	if (!_CheckValidValueHeader(Str, Pos + 1, Str.length()))
		return (false);

	HelperFunctions::ConvertStringToLower(Str, Pos);
	HelperFunctions::CopyStr(Str,_NameHeader, 0, Pos);
	HelperFunctions::CopyStr(Str,_ValueHeader, Pos + 1, Str.length());
	if (!_NameHeader.compare("status"))
		if (!_ParseStatus(_ValueHeader))
			return false;
	if (!_NameHeader.compare("content-type"))
			_ExistHeaders[stHeadersCGI::CONTENT_TYPE] = stHeadersCGI::CONTENT_TYPE;
	else if (!_NameHeader.compare("location"))
		_ExistHeaders[stHeadersCGI::LOCATION] = stHeadersCGI::LOCATION;
	else if (!_NameHeader.compare("status"))
		_ExistHeaders[stHeadersCGI::STATUS] = stHeadersCGI::STATUS;
	else if (!_NameHeader.compare("date") || !_NameHeader.compare("server")
		|| !_NameHeader.compare("connection") || !_NameHeader.compare("transfer-encoding"))
	{
		_NameHeader = "";
		_ValueHeader = "";
		return true;
	}
	if (!_StoredHeadersField(Str))
		return false;
	return true;
}

void clsParseOutCGI::_Connection(bool Isclose)
{
	if (Isclose)
		_HeadersFieldFinal += "Connection: keep-alive\r\n";
	else
		_HeadersFieldFinal += "Connection: Close\r\n";
}

void clsParseOutCGI::_StatusNormal()
{
	if (_HeadersField.count("status"))
	{
		_HeadersFieldFinal +=  "HTTP/1.1 ";
		_HeadersFieldFinal +=_HeadersField["status"];
		_HeadersFieldFinal += "\r\n";
	}
	else
		_HeadersFieldFinal+=  "HTTP/1.1 200 OK\r\n";
}

void clsParseOutCGI::_StatusRedirection()
{
	if (_ExistHeaders[stHeadersCGI::STATUS] != stHeadersCGI::STATUS)
	{
		_HeadersFieldFinal +=  "HTTP/1.1 ";
		_HeadersFieldFinal +=_HeadersField["status"];
		_HeadersFieldFinal += "\r\n";
	}
	else
	{
		_HeadersFieldFinal += "HTTP/1.1 ";
		_HeadersFieldFinal += "302 ";
		_HeadersFieldFinal += HelperFunctions::GetStatusMessage(302);
		_HeadersFieldFinal +="\r\n";
	}
}

void clsParseOutCGI::_HeaderResponseCGI()
{
	std::map <std::string, std::string>::iterator HEAD;
	if (_Mod[stMod::REDIRECTION] != stMod::REDIRECTION)
		_StatusNormal();
	else
		_StatusRedirection();
	for(HEAD = _HeadersField.begin(); HEAD != _HeadersField.end(); HEAD++)
	{
		if (HEAD->first.compare("status") != 0)
		{
			_HeadersFieldFinal += HEAD->first ;
			_HeadersFieldFinal +=":" ;
			_HeadersFieldFinal += HEAD->second ;
			_HeadersFieldFinal += "\r\n";
		}
	}
	_HeadersFieldFinal += _HeadersFieldDuplicate;
	if (_Mod[stMod::CHUNK] == stMod::CHUNK)
		_Transfer_Encoding();
	if (_Mod[stMod::CHUNK] != stMod::CHUNK)
		_ContentLength();
	_Date();
	_Server();
	_Connection(_IsConnectoin);
	_HeadersFieldFinal += "\r\n";
}

void clsParseOutCGI::_InitialInternalRedirect()
{
	short Pos = 0;
	_ValueHeader = _HeadersField["location"];
	short Skeep = HelperFunctions::SkeeSep(_ValueHeader, " \t");
	if (_ValueHeader[Skeep] != '/')
	{
		_Mod[stMod::ERROR] = stMod::ERROR;
		_Status = 502;
		return ;
	}
	Pos = HelperFunctions::SkeepAtLast(_ValueHeader," \t");
	if (Pos < 0)
		Pos = _ValueHeader.size();
	else
		Pos++;
	if (_ValueHeader.size() + _InternalRedirectSrc.size() + 4 > MAX_HEADERS)
	{
		_Status = 413;
		_Mod[stMod::ERROR] = stMod::ERROR;
		return ;
	}
	_InternalRedirectSrc += "GET ";
	HelperFunctions::CopyStr(_ValueHeader,_InternalRedirectSrc, Skeep, Pos);
	if (_InternalRedirectSrc.size() + 11 > MAX_HEADERS)
	{
		_Status = 413;
		_Mod[stMod::ERROR] = stMod::ERROR;
		return ;
	}
	_InternalRedirectSrc += " HTTP/1.1\r\n";
}

void clsParseOutCGI::_BuilResponsedredirection()
{
	uint8_t CountValidHeader = 0;
	if (_ExistHeaders[stHeadersCGI::STATUS] == stHeadersCGI::STATUS)
		CountValidHeader++;
	if (_ExistHeaders[stHeadersCGI::CONTENT_TYPE] == stHeadersCGI::CONTENT_TYPE)
		CountValidHeader++;
	if (_LocationIsClientOrLocal(_HeadersField["location"]) && CountValidHeader == 2 && _BytesBody)
		_HeaderResponseCGI();
	else if (!CountValidHeader && _LocationIsClientOrLocal(_HeadersField["location"]) && !_BytesBody)
		_HeaderResponseCGI();
	else if(_HeadersField.size() == 1 && !_BytesBody)
		_InitialInternalRedirect();
	else
	{
		_Mod[stMod::ERROR] = stMod::ERROR;
		_Status = 502;
	}
}

void clsParseOutCGI::_ReceivingHeaders(const char *Arr, short Length)
{
	_Counter = 0;
	if (_FoundBody)
		return ;
	bool Flag = false;
	HelperFunctions::GetCleanLineHeader(Arr, _Line, _CountSizeHeaders, Flag, _Counter, Length);
	if (_CountSizeHeaders > MAX_HEADERS / 2)
	{
		_Status = 413;
		_Mod[stMod::ERROR] = stMod::ERROR;
		return ;
	}
	while(Flag)
	{
		Flag = false;
		if (_CountSizeHeaders > MAX_HEADERS / 2)
		{
			_Status = 413;
			_Mod[stMod::ERROR] = stMod::ERROR;
			return ;
		}
		if (_Line.compare("\r\n"))
		{
			if (!_ValidHeaders(_Line))
			{
				_Status = 502;
				_Mod[stMod::ERROR] = stMod::ERROR;
				return ;
			}
		}
		else
		{
			_FoundBody = true;
			if (_ExistHeaders[stHeadersCGI::CONTENT_TYPE] != stHeadersCGI::CONTENT_TYPE &&
					_ExistHeaders[stHeadersCGI::LOCATION] != stHeadersCGI::LOCATION &&
						_ExistHeaders[stHeadersCGI::STATUS] != stHeadersCGI::STATUS)
			{
				_Status = 502;
				_Mod[stMod::ERROR] = stMod::ERROR;
			}
			return;
		}
		_Line = "";
		HelperFunctions::GetCleanLineHeader(Arr, _Line, _CountSizeHeaders, Flag, _Counter, Length);
	}
}

void clsParseOutCGI::_CreatFileTemp()
{
	char Arr[20] =  "/tmp/CGIout.XXXXXX\0";
	_Fdout = mkstemp(Arr);
	if (_Fdout == -1)
	{
		_Status = 500;
		_Mod[stMod::ERROR] = stMod::ERROR;
		return ;
	}
	_FileNameFromDisk =  Arr;
}

void clsParseOutCGI::_ReceivingBody(const char *Arr, short Length)
{
	if (_FoundBody && _BytesBody < MAX_BODY)
	{
	   _BytesBody += ((Length - _Counter  < 0)? 0 : Length - _Counter);
	   if (_ExistHeaders[stHeadersCGI::CONTENT_TYPE]  != stHeadersCGI::CONTENT_TYPE && _BytesBody)
		{
			_Status = 502;
			_Mod[stMod::ERROR] = stMod::ERROR;
			return ;
		}
	   if (_BytesBody > MAX_BODY)
	   {
			
			_Mod[stMod::CHUNK] = stMod::CHUNK;
			_CreatFileTemp();
			if (_Mod[stMod::ERROR] == stMod::ERROR)
				return ;
			if (write(_Fdout, &_Body[0], _OffsetBody) == -1)
			{
				_Status = 500;
				_Mod[stMod::ERROR] = stMod::ERROR;
				return ;
			}
			if (write(_Fdout, &Arr[_Counter], ((Length - _Counter  < 0)? 0 : Length - _Counter)) == -1)
			{
				_Status = 500;
				_Mod[stMod::ERROR] = stMod::ERROR;
				return ;
			}
	   }
	   else
			HelperFunctions::ft_str_copy(&_Body[0], &Arr[_Counter], MAX_BODY, _OffsetBody, ((Length - _Counter  < 0)? 0 : Length - _Counter), 0);
	}
	else if (_BytesBody > MAX_BODY)
	{
		_BytesBody += Length;
		if (write(_Fdout, Arr, Length) == -1)
		{
			_Status = 500;
			_Mod[stMod::ERROR] = stMod::ERROR;
			return ;
		}
	}
}

void clsParseOutCGI::ReceivingData(const char *Arr, short Length)
{
	if (_Mod[stMod::MEMORY_FAILD] == stMod::MEMORY_FAILD)
		return ;
	_ReceivingHeaders(Arr, Length);
	_ReceivingBody(Arr, Length);
	if (_Mod[stMod::ERROR] == stMod::ERROR)
	{
		if (_Fdout != -1)
		{
			close(_Fdout);
			_Fdout = -1;
		}
		return ;
	}
	if (!_ProcessIsFinish)
		return ;
	else if (!_FoundBody || (!_BytesBody &&
			_ExistHeaders[stHeadersCGI::CONTENT_TYPE] == stHeadersCGI::CONTENT_TYPE))
	{
		_Status = 502;
		_Mod[stMod::ERROR] = stMod::ERROR;
		return ;
	}
	else
	{
		if (_Fdout != -1)
		{
			close(_Fdout);
			_Fdout = -1;
		}
	}
	if (_ExistHeaders[stHeadersCGI::LOCATION] == stHeadersCGI::LOCATION)
	{
		_Mod[stMod::REDIRECTION] = stMod::REDIRECTION;
		_BuilResponsedredirection();
	}
	else
		_HeaderResponseCGI();
}

void clsParseOutCGI::_Transfer_Encoding()
{
	_HeadersFieldFinal += "Transfer-Encoding: chunked\r\n";
}

void clsParseOutCGI::_Date()
{
	_HeadersFieldFinal += "Date: ";
	HelperFunctions::DateTime(_HeadersFieldFinal);
	_HeadersFieldFinal += "\r\n";
}

void clsParseOutCGI::_Server()
{
	_HeadersFieldFinal += "Server: the-fastest-server\r\n";
}

void clsParseOutCGI::_ContentLength()
{
	_HeadersFieldFinal += "Content-Length: ";
	HelperFunctions::NumToStr(_BytesBody, _HeadersFieldFinal);
	_HeadersFieldFinal +="\r\n";
}

const std::string &clsParseOutCGI::GetHeadersFieldFinal()
{
	return _HeadersFieldFinal;
}


void clsParseOutCGI::SetProcessIsFinish(bool ProcessIsFinish)
{
	_ProcessIsFinish = ProcessIsFinish;
}

const std::string &clsParseOutCGI::GetBody()
{
	return _Body;
}

const std::string &clsParseOutCGI::GetFileNameBody()
{
	return _FileNameFromDisk;
}

bool clsParseOutCGI::GetModTransferData() const
{
	return _ModTransferData;
}

stMod::eMod *clsParseOutCGI::GetMod()
{
	return _Mod;
}

short clsParseOutCGI::GetStatus()
{
	return _Status;
}

std::string &clsParseOutCGI::GetInternalRedirectSrc()
{
	return _InternalRedirectSrc;
}

void clsParseOutCGI::Reset()
{
	if (!_HeadersField.empty())
		_HeadersField.clear();
	_HeadersFieldDuplicate = "";
	_Line = "";
	_BytesBody = 0;
	_Status = 0;
	_IsConnectoin = true;
	_FoundBody = false;
	_ProcessIsFinish = false;
	_NameHeader = "";
	_OffsetBody = 0;
	_ValueHeader = "";
	_CountSizeHeaders = 0;
	_ModTransferData = 0;
	_Fdout = -1;
	close(_Fdout);
	HelperFunctions::ft_memset(_Mod, stMod::EMPTY, sizeof(_Mod));
	HelperFunctions::ft_memset(_ExistHeaders, stHeadersCGI::EMPTY, sizeof(_ExistHeaders));
}

size_t clsParseOutCGI::GetSizeBody()
{
	return _BytesBody;
}
bool clsParseOutCGI::GetIsConnection()
{
	return _IsConnectoin;
}
clsParseOutCGI::~clsParseOutCGI()
{
	if (_Fdout != -1)
		close(_Fdout);
}