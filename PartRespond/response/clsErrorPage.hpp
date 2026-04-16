/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsErrorPage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:33:05 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/16 14:48:40 by achamdao         ###   ########.fr       */
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
        int _BodySize;
        bool _Erno;

        void _HeadersErrorResponse();
        void _StoredBodys();
        void _StoredMessage();
        void _Status();
        void _ContentLength();
        void _ContentType();
        void _ConnectionClose();
        void _StoredInFileOrStr();
        void _Date();
        void _Server();
        void _Allow();
        void _RetryAfter();
        void _Transfer_Encoding();
        void _StoredInFileOrStr()
    public:
        clsErrorPage();
        void SetType(std::string Type);
        void SetBodySize(int BodySize);
        void SetMod(const stMod::eMod *Mod);
        std::string GetType();
        void ResponseError(int Status, const std::string &FilePageError);
        ~clsErrorPage();
};
#endif
