/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:25 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/20 22:07:09 by achamdao         ###   ########.fr       */
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
        std::string GetTypeData(std::string Type);
        std::string ErrorRespnseHandling();
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
        void Reset();
    public:
        clsResponse();
        
        std::string GetBody();
        std::string GetFileName();
        std::string GetHeaderFeild();
        void SetStatus(short Status);
        void SetRequestHandler(RequestHandler DataRequest);
        void SetFileFromDisk(std::string FileFromDisk);
        void SetMod(short Mode);
        void SetType(std::string Type);
        bool GetIsConnection();
        void MakeResponse();
        std::string ChunkData(const std::string &Str);

        ~clsResponse();
};
#endif
