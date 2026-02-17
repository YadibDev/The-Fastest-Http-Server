/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsErrorPage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 14:33:05 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/17 14:50:13 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_ERROR_PAGE_HPP
# define CLS_ERROR_PAGE_HPP

#include "../mainprocess/librarys.hpp"

class clsErrorPage
{
    private:
        int _Status;
        std::string _Type;
        std::map<int, std::string> _Message;
        std::map<int, std::string> _Body;
        std::string _HeaderFeild;
        int _BodySize;

        std::string HeadersErrorResponse();
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
    public:
        clsErrorPage();
        void SetType(std::string Type);
        std::string GetType();
        std::string GetStatusMessage(int StatusCode);
        std::string GetBody(int StatusCode);
        std::string ResponseError(int Status);
        ~clsErrorPage();
};
#endif
