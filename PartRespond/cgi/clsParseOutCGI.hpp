/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:48 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/09 10:26:35 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_PARSE_OUT_CGI_HPP
# define CLS_PARSE_OUT_CGI_HPP

#include "../mainprocess/librarys.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"
#include "../response/clsErrorPage.hpp"

class clsParseOutCGI
{
        std::string _Data;
        stMod::eMod _Mod[10];
        bool _FoundBody;
        int _Status;
        int _BytesBody;
        uint8_t _CounterCGI_Field;
        bool _ProcessIsFinish;
        std::string _Body;
        std::string _FileNameFromDisk;
        clsErrorPage _ErrorPage;
        RequestHandler _DataRequest;
        short _CountSizeHeaders;
        short _MaxSizeHeaders;
        int _MaxSizeBody;
        short _CountFileTemp;
        std::map <std::string, std::string> _HeadersField;
        std::string _HeadersFieldDuplicate;
        std::string _HeadersFieldFinal;
        std::string _Line;
        std::string _NameFileBody;
        bool _Erno;
        int _Pipe_Fd;
        int _Fdout;
        bool CheckValidNameHeader(std::string &HeaderName, short Start, short End);
        void ErrorRespnseHandling();
        bool LocationIsClientOrLocal(std::string &Location);
        bool ParseStatus(const std::string &StatusLineValue);
        bool MakeHeadersResponse(std::string &StatusLine);
        bool IsSpecialChar(char C);
        bool CheckValidValueHeader(std::string &HeaderValue, short Start, short End);
        void StoredBlackListHeaders(std::vector <std::string> &BalckListHeader);
        bool ValidHeaders(std::string &Str);
        bool ParseContentType(const std::string &ValueContentType);
        void Connection(bool Isclose);
        void BuilResponsedredirection();
        void HeaderResponseCGI();
        void Transfer_Encoding();
        void Date();
        void CachControl();
        void Server();
        void StatusNormal();
        void StatusRedirection();
        void ContentLength();
        void ReceivingHeaders(std::string &Data);
        void ReceivingBody(std::string &Data);
        bool StoredHeadersField(std::string &Name, std::string &Value, std::string &Str);
        void StoredInFileOrStr();
        void _CreatFileTemp();
    public:
        clsParseOutCGI();
        const std::string &GetBody();
        const std::string &GetFileNameBody();
        const std::string &GetHeadersFieldFinal();
        void SetPipe_Fd(int Pipe_Fd);
        void ReceivingData(std::string &Data);
        void SetProcessIsFinish(bool ProcessIsFinish);
        ~clsParseOutCGI();
};
#endif