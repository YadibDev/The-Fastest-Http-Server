/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsErrorPage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:33:05 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/04 21:33:36 by achamdao         ###   ########.fr       */
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
        std::string _HeaderFeild;
        int _BodySize;

        std::string &HeadersErrorResponse();
        void StoredBodys();
        void StoredMessage();
        void Status();
        void ContentLength();
        void ContentType();
        void ConnectionClose();
        void StoredInFileOrStr();
        void Date();
        void Server();
        void Allow();
        void RetryAfter();
        void Transfer_Encoding();
    public:
        clsErrorPage();
        void SetType(std::string Type);
        void SetBodySize(int BodySize);
        void SetMod(const stMod::eMod *Mod);
        std::string GetType();
        std::string &ResponseError(int Status);
        ~clsErrorPage();
};
#endif
