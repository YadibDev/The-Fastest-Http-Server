/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yadib <yadib@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:25 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/25 17:02:29 by yadib            ###   ########.fr       */
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
        short _MaxSizeHeader;
        int _MaxSizeBody;
        int _BodySize;
        int _SizeHeaders;
        bool _IsConnection;
        bool _Erno;
        bool _ModTransferData;
        bool _InternalRedirect;
        std::string _InternalRedirectSrc;
        std::string _Type;
        clsErrorPage _ErrorPage;
        std::string _Body;
        std::string _HeaderFeild;
        std::string _FileFromDisk;
        const std::string *_BodyPointer;
        const std::string *_HeaderFeildPointer;
        const std::string *_FileFromDiskPointer;
        const RequestHandler &_DataRequest;

        const std::string GetTypeData(const std::string &Type);
        void _ErrorRespnseHandling();
        void _InitialHeaders();
        void _StatusLine();
        void _ContentLength();
        void _ContentType();
        void _Connection(bool Isclose);
        void _Redirction();
        void _Transfer_Encoding();
        void _StoredInFileOrStr();
        void _Date();
        void _CachControl();
        void _Server();
    public:
        void Reset();
        clsResponse(RequestHandler &_DataRequest);
        const std::string &GetBody() const;
        const std::string &GetFileName() const;
        const std::string &GetHeaderFeild() const;
        void SetStatus(short Status);
        void SetMod(stMod::eMod Mod);
        bool GetIsConnection() const;
        void MakeResponse();
        bool GetErnoVar();
        const std::string *GetBodyPointer();
        const std::string *GetHeaderFeildPointer();
        const std::string *GetFileFromDiskPointer() const;
        void SetBodyPointer(const std::string *BodyPointer);
        void SetHeaderFeildPointer(const std::string *HeaderFeildPointer);
        void SetFileFromDiskPointer(const char *FileFromDiskPointer);
        void SetFileFromDiskPointer(const std::string *FileFromDiskPointer);
        void ChunkData(std::string &NewStr, const std::string &Str, bool lastChunked) const;
        void SetModTransferData(bool ModTransferData);
        bool GetModTransferData() const;
        int GetSizeBody() const;
        void SetSizeBody(int size);

        ~clsResponse();
};
#endif
