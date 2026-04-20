/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/16 10:25:37 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_CGI_HPP
# define CLS_CGI_HPP

#include "clsParseOutCGI.hpp"
#include "../mainprocess/librarys.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"

class clsCGI
{
    private:
        pid_t _PIDCHILD;
        bool _IsRunCGI;
        long long _StartTime;
        int _FD;
        const RequestHandler &_DataRequest;
        clsParseOutCGI _ParseOutCGI;
        char **_ENV;
        char **_ARG;
        int _pip[2];
        std::string _BuildVarEnv(const std::string &HeaderName,const std::string  &Value);
        bool _childeProcesse();
        int _ParentProcesse();
        bool _InintialVar();
        bool _StoredArgs();
        bool _MakeEnv();
    public:
        clsCGI(const RequestHandler &DataRequest);
        bool GetIsRunCGI();
        clsParseOutCGI &GetclsParseOutCGI();
        int RunCGI();
        int GetPid();
        void SetIsRunCGI(bool IsRunCGI);
        ~clsCGI();
};

#endif