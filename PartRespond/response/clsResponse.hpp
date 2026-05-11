/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:25 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/11 21:02:16 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_RESPONSE_HPP
# define CLS_RESPONSE_HPP

#include "../mainprocess/librarys.hpp"
#include "../response/clsErrorPage.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"
#include "clsAutoIndex.hpp"

class clsResponse
{
    private:
        stMod::eMod _Mod[10];
        short _Status;
        size_t _BodySize;
        int _SizeHeaders;
        bool _IsConnection;
        bool _Erno;
        bool _ModTransferData;
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
        clsAutoIndex AutoIndex;

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
        void _Server();
        void _InitialAutoIndex();
        void _DeleteResource();
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
        void SetInternalRedirectSrc(const std::string &InternalRedirectSrc);
        std::string &GetInternalRedirectSrc();
        bool GetModTransferData() const;
        size_t GetSizeBody() const;
        void SetSizeBody(int size);
        bool IsAutoIndex();
        void SetIsConnection(bool IsConnection);
        bool fetchAutoIndex(char * Buffer, short &Ofset, short LimitSize);
        short GetStatus();
        bool IsError();

        ~clsResponse();
};
#endif
