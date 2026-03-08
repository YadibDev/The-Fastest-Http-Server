/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:48 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/08 19:38:29 by achamdao         ###   ########.fr       */
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
        int _SizeFile;
        std::string _Body;
        std::string _RemaindData;
        clsErrorPage _ErrorPage;
        std::map <std::string, std::string> _HeadersField;
        std::string _HeadersFieldFinal;
        std::map <std::string, std::string> _SpecialHeaders;
        std::vector <std::string> _BlackListHeaders;
        std::string _NameFileBody;
        int _Pipe_Fd;
    public:
        clsParseOutCGI();
        void SetPipe_Fd(int Pipe_Fd);
        const std::string &GetHeadersFieldFinal() const;
        bool CheckValidNameHeader(std::string &HeaderName, short Start, short End);
        void ReceivingData(std::string &Data);
        bool LocationIsClientOrLocal(std::string &Location);
	    bool ParseStatus(const std::string &StatusLineValue);
	    bool MakeHeadersResponse(std::string &StatusLine);
        bool IsSpecialChar(char C);
        bool CheckValidValueHeader(std::string &HeaderValue, short Start, short End);
	    void StoredBlackListHeaders(std::vector <std::string> &BalckListHeader);
        bool ValidHeaders(std::string &Str);
        bool ParseContentType(const std::string &ValueContentType);
        std::string Connection(bool Isclose);
        void BuilResponsedredirection();
        void HeaderResponseCGI();
        void Transfer_Encoding();
        void Date();
        void CachControl();
        void Server();
        void StatusNormal();
        void StatusRedirection();
        void ContentLength();
        ~clsParseOutCGI();
};
#endif