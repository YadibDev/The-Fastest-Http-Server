/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:25 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/21 17:05:51 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_RESPONSE_HPP
# define CLS_RESPONSE_HPP

#include "../mainprocess/librarys.hpp"
#include "../response/clsErrorPage.hpp"
#include "RequestHandler.hpp"

class clsResponse
{
    private:
        std::map <short, short> _Mod;
        short _Status;
        short _BodySize;
        bool _IsConnection;
        bool _Erno;
        std::string _Body;
        std::string _Type;
        clsErrorPage _ErrorPage;
        std::string _HeaderFeild;
        std::string  _FileName;
        std::string  _FileFromDisk;
        RequestHandler _DataRequest;
        std::map<std::string, std::string> _TypeContent;

        void StoredDefaultType();
        const std::string &GetTypeData(const std::string &Type);
        const std::string &ErrorRespnseHandling();
        void InitialHeaders();
        void Status();
        void ContentLength();
        void ContentType();
        void ConnectionKeepAlive();
        void ConnectionClose();
        void Redirction();
        void Transfer_Encoding();
        void StoredInFileOrStr();
        void Date();
        void CachControl();
        void Server();
    public:
        void Reset();
        clsResponse();
        const std::string &GetBody();
        const std::string &GetFileName();
        const std::string &GetHeaderFeild();
        void SetStatus(short Status);
        void SetRequestHandler(const RequestHandler &DataRequest);
        void SetFileFromDisk(const std::string &FileFromDisk);
        void SetMod(short Mode);
        void SetType(const std::string &Type);
        bool GetIsConnection();
        void MakeResponse();
        const std::string &ChunkData(const std::string &Str);

        ~clsResponse();
};
#endif
