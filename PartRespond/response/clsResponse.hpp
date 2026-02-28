/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:25 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/28 22:21:28 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_RESPONSE_HPP
# define CLS_RESPONSE_HPP

#include "../mainprocess/librarys.hpp"
#include "../response/clsErrorPage.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"




class clsResponse
{
    private:
        std::map <short, short> _Mod;
        stMod::eMod Mod[10];
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
        const std::string GetTypeData(const std::string &Type);
        const std::string ErrorRespnseHandling();
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
        const std::string &GetBody() const;
        const std::string &GetFileName() const;
        const std::string &GetHeaderFeild() const;
        void SetStatus(short Status);
        void SetRequestHandler(const RequestHandler &DataRequest);
        void SetFileFromDisk(const std::string &FileFromDisk);
        void SetMod(short Mode);
        void SetType(const std::string &Type);
        bool GetIsConnection() const;
        void MakeResponse();
        void clsResponse::ChunkData(std::string &NewStr, const std::string &Str, bool lastChunked) const;

        ~clsResponse();
};
#endif
