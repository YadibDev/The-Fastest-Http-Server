/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsParseOutCGI.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:48 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/15 09:39:59 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_PARSE_OUT_CGI_HPP
# define CLS_PARSE_OUT_CGI_HPP

#include "../mainprocess/librarys.hpp"

#include "../response/clsErrorPage.hpp"

class clsParseOutCGI
{
        std::string _Data;
        short _Mod;
        bool _FoundBody;
        int _Status;
        int _BytesBody;
        int _SizeFile;
        std::string _Body;
        clsErrorPage ErrorPage;
        std::map <std::string, std::string> _HeadersField;
        std::map <std::string, std::string> _SpecialHeaders;
        std::vector <std::string> _BlackListHeaders;
        int _FD;
    public:
        clsParseOutCGI();
        bool CheckValidNameHeader(std::string &Headrs);
        void ReceivingData(std::string &Data);
        bool LocationIsClientOrLocal(std::string &Location);
	    bool ParseStatus(const std::string &StatusLineValue);
	    bool MakeHeadersResponse(std::string &StatusLine);
        bool IsSpecialChar(char C);
        bool CheckValidValueHeader(std::string &HeaderValue);
	    void StoredBlackListHeaders(std::vector <std::string> &BalckListHeader);
        bool ValidHeaders(std::string &Str, std::vector <std::string> &Data);
        bool StoredCleanHeaders(std::string &Str);
        bool ParseContentType(const std::string &ValueContentType);
        std::string Connection(bool Isclose);
        std::string BuilResponsedredirection();
        std::string HeaderResponseCGI();
};
#endif