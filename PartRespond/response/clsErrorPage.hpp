/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsErrorPage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:33:05 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/06 20:53:57 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_ERROR_PAGE_HPP
# define CLS_ERROR_PAGE_HPP

#include "../mainprocess/librarys.hpp"

class clsErrorPage
{
    private:
        int _Status;
        stMod::eMod _Mod[10];
        std::string _Type;
        std::string _Body;
        std::string _HeaderFeild;
        std::string _FileFromDisk;
        bool _IsConnection;
        bool _IsAutoIndex;
        size_t _BodySize;
        bool _Erno;

        void _HeadersErrorResponse();
        void _StoredBodys();
        void _StoredMessage();
        void _StatusLine();
        void _ContentLength();
        void _ContentType();
        void _Connection();
        void _Date();
        void _Server();
        void _Allow();
        void _RetryAfter();
        void _Transfer_Encoding();
        void _StoredInFileOrStr(size_t sizeBody);
        void _CheckConnection();
    public:
        clsErrorPage();
        void SetBodySize(int BodySize);
        size_t GetBodySize() const;
        const std::string &GetHeaderField();
        const std::string &GetBody();
        const std::string &GetFileFromDisk();
        void ResponseError(int Status, const std::string &FilePageError, size_t sizeBody);
        bool GetIsConnection();
        void Reset();
        void SetAutoIndex(bool IsAutoIndex);
        ~clsErrorPage();
};
#endif
