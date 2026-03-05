/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:25 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/05 21:33:32 by achamdao         ###   ########.fr       */
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
        stMod::eMod _Mod[10];
        short _Status;
        int _BodySize;
        int _SizeHeaders;
        bool _IsConnection;
        bool _Erno;
        char *_Body;
        std::string _Type;
        clsErrorPage _ErrorPage;
        char *_HeaderFeild;
        std::string _FileName;
        std::string _FileFromDisk;
        RequestHandler _DataRequest;

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
        void SetMod(stMod::eMod Mod);
        void SetType(const std::string &Type);
        bool GetIsConnection() const;
        void MakeResponse();
        void clsResponse::ChunkData(std::string &NewStr, const std::string &Str, bool lastChunked) const;

        ~clsResponse();
};
#endif
