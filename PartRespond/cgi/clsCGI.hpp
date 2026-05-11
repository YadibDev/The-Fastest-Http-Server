/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/11 11:56:15 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_CGI_HPP
# define CLS_CGI_HPP

#include "clsParseOutCGI.hpp"
#include "../mainprocess/librarys.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"

# define SIZE_VAR_ENV  (17 + 31 + 1)

class clsCGI
{
    private:
        pid_t _PIDCHILD;
        bool _IsRunCGI;
        time_t _StartTime;
        int _FD;
        bool _Erno;
        static short _LimitProcess;
        short _Offset;
        char *_Buffer;
        const char *_PortServer;
        const char *_IPClient;
        RequestHandler &_DataRequest;
        clsParseOutCGI _ParseOutCGI;
        std::string TempVar;
        char *_ENV[SIZE_VAR_ENV];
        char *_ARG[3];
        int _pip[2];
        uint8_t _Counter;
        std::string _BuildVarEnv(const std::string &HeaderName,const std::string  &Value);
        bool _childeProcesse();
        void _ParentProcesse();
        bool _InintialVar();
        bool _StoredArgs();
        bool _MakeEnv();
        bool _SERVER_SOFTWARE();
        bool _SERVER_NAME();
        bool _GATEWAY_INTERFACE();
        bool _SERVER_PROTOCOL();
        bool _SERVER_PORT();
        bool _REQUEST_METHOD();
        bool _PATH_INFO();
        bool _PATH_TRANSLATED();
        bool _SCRIPT_NAME();
        bool _QUERY_STRING();
        bool _REMOTE_HOST();
        bool _REMOTE_ADDR();
        bool _AUTH_TYPE();
        bool _REMOTE_USER();
        bool _REMOTE_IDENT();
        bool _CONTENT_TYPE();
        bool _CONTENT_LENGTH();
        bool _OtherHeaders();
    public:
        clsCGI(RequestHandler &DataRequest);
        bool GetIsRunCGI();
        bool GetErno();
        int GetFdPipe();
        clsParseOutCGI &GetclsParseOutCGI();
        void RunCGI();
        int GetPid();
        void SetIsRunCGI(bool IsRunCGI);
        const time_t &getStartTime() const;
        void Reset();
        void SetBuffer(char *Buffer);
        void SetPortS_and_IPC(const char *IPC, const char *PosrtS);
        ~clsCGI();
};

#endif