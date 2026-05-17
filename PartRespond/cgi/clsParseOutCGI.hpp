/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:48 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/17 18:27:15 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLS_PARSE_OUT_CGI_HPP
#define CLS_PARSE_OUT_CGI_HPP

#include "../mainprocess/librarys.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"
#include "../response/clsErrorPage.hpp"

struct stHeadersCGI
{
    enum eHeaders
    {
        EMPTY,
        CONTENT_TYPE,
        LOCATION,
        STATUS
    };
};

class clsParseOutCGI
{
    stMod::eMod _Mod[11];
    stHeadersCGI::eHeaders _ExistHeaders[4];
    bool _FoundBody;
    short _Status;
    short _Counter;
    size_t _BytesBody;
    bool _IsConnectoin;
    bool _ProcessIsFinish;
    const RequestHandler &_DataRequest;
    std::string &_Body;
    std::string &_HeadersFieldFinal;
    std::string &_FileNameFromDisk;
    std::string &_InternalRedirectSrc;
    std::string _NameHeader;
    std::string _ValueHeader;
    short _CountSizeHeaders;
    short _OffsetBody;
    std::map<std::string, std::string> _HeadersField;
    std::string _HeadersFieldDuplicate;
    std::string _Line;
    bool _ModTransferData;
    int _Fdout;
    bool _CheckValidNameHeader(std::string &HeaderName, short Start, short End);
    bool _LocationIsClientOrLocal(std::string &Location);
    bool _ParseStatus(const std::string &StatusLineValue);
    bool _MakeHeadersResponse(std::string &StatusLine);
    bool _IsSpecialChar(char C);
    bool _CheckValidValueHeader(std::string &HeaderValue, short Start, short End);
    void _StoredBlackListHeaders(std::vector<std::string> &BalckListHeader);
    bool _ValidHeaders(std::string &Str);
    bool _ParseContentType(const std::string &ValueContentType);
    void _Connection(bool Isclose);
    void _BuilResponsedredirection();
    void _HeaderResponseCGI();
    void _Transfer_Encoding();
    void _Date();
    void _Server();
    void _StatusNormal();
    void _StatusRedirection();
    void _ContentLength();
    void _ReceivingHeaders(const char *Arr, short Length);
    void _ReceivingBody(const char *Arr, short Length);
    bool _StoredHeadersField(std::string &Str);
    void _CreatFileTemp();
    bool _IsValidHeaderValueChar(unsigned char C);
    void _InitialInternalRedirect();
    short _AtoiStatusCode(const std::string &StringDigit, short Start, short End);

public:
    clsParseOutCGI(RequestHandler &_DataRequest,std::string &Body,  std::string &HeadersFieldFinal, std::string &_FileNameFromDisk, std::string &InternalRedirectSrc);
    const std::string &GetBody();
    const std::string &GetFileNameBody();
    const std::string &GetHeadersFieldFinal();
    stMod::eMod *GetMod();
    void ReceivingData(const char *Arr, short Length);
    void SetProcessIsFinish(bool ProcessIsFinish);
    bool GetModTransferData() const;
    std::string &GetInternalRedirectSrc();
    short GetStatus();
    bool GetIsConnection();
    size_t GetSizeBody();
    void Reset();

    ~clsParseOutCGI();
};
#endif