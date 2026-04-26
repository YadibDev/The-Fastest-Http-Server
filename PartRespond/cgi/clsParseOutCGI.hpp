/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:48 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/18 10:55:51 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_PARSE_OUT_CGI_HPP
# define CLS_PARSE_OUT_CGI_HPP

#include "../mainprocess/librarys.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"
#include "../response/clsErrorPage.hpp"

struct stHeadersCGI
{
    enum eHeaders{CONTENT_TYPE,LOCATION,STATUS, EMPTY = -1};
};

class clsParseOutCGI
{
        std::string _Data;
        stMod::eMod _Mod[10];
        pid_t _PIDCHILD;
        stHeadersCGI::eHeaders _ExistHeaders[3];
        bool _FoundBody;
        bool _Erno;
        std::string _InternalRedirectSrc;
        short _Status;
        int _BytesBody;
        uint8_t _CounterCGI_Field;
        bool _ProcessIsFinish;
        std::string _Body;
        std::string _NameHeader;
        std::string _ValueHeader;
        std::string _FileNameFromDisk;
        clsErrorPage _ErrorPage;
        const RequestHandler &_DataRequest;
        short _CountSizeHeaders;
        std::map <std::string, std::string> _HeadersField;
        std::string _HeadersFieldDuplicate;
        std::string _HeadersFieldFinal;
        std::string _Line;
        std::string _NameFileBody;
        const std::string *_BodyPointer;
        const std::string *_HeaderFeildPointer;
        const std::string *_FileFromDiskPointer;
        bool _ModTransferData;
        int _Pipe_Fd;
        int _Fdout;
        bool _CheckValidNameHeader(std::string &HeaderName, short Start, short End);
        void _ErrorRespnseHandling();
        bool _LocationIsClientOrLocal(std::string &Location);
        bool _ParseStatus(const std::string &StatusLineValue);
        bool _MakeHeadersResponse(std::string &StatusLine);
        bool _IsSpecialChar(char C);
        bool _CheckValidValueHeader(std::string &HeaderValue, short Start, short End);
        void _StoredBlackListHeaders(std::vector <std::string> &BalckListHeader);
        bool _ValidHeaders(std::string &Str);
        bool _ParseContentType(const std::string &ValueContentType);
        void _Connection(bool Isclose);
        void _BuilResponsedredirection();
        void _HeaderResponseCGI();
        void _Transfer_Encoding();
        void _Date();
        void _CachControl();
        void _Server();
        void _StatusNormal();
        void _StatusRedirection();
        void _ContentLength();
        void _ReceivingHeaders(const char *Arr, short Length);
        void _ReceivingBody(const char *Arr, short Length);
        bool _StoredHeadersField(std::string &Str);
        void _StoredInFileOrStr();
        void _CreatFileTemp();
        bool _IsValidHeaderValueChar(unsigned char C);
        void _InitialInternalRedirect();
        void _Reset();
    public:
        clsParseOutCGI(const RequestHandler &_DataRequest);
        const std::string &GetBody();
        const std::string &GetFileNameBody();
        const std::string &GetHeadersFieldFinal();
        const std::string *GetBodyPointer();
        const std::string *GetHeaderFeildPointer();
        const std::string *GetFileFromDiskPointer();
        void SetPipe_Fd(int Pipe_Fd);
        stMod::eMod *GetMod();
        void ReceivingData(const char *Arr, short Length);
        void SetProcessIsFinish(bool ProcessIsFinish);
        void SetPIDPROCESS(int PIDPROCESS);
        bool GetModTransferData() const;
        bool GetErno();
        std::string &GetInternalRedirectSrc();
        short GetStatus();
        short GetSizeBody();
        
        ~clsParseOutCGI();
};
#endif