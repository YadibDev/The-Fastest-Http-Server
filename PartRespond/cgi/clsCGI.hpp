/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/03/12 16:35:06 by achamdao         ###   ########.fr       */
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
        // RequestHandler _DataRequest;
        clsParseOutCGI _ParseOutCGI;
        std::map<std::string, bool> _WhiteBlakHeaders;
        void _StoredWhiteBlakHeaders();
        const std::string  &_ConcatonateValue(const std::vector <std::string> &Value);
        std::string _BuildVarEnv(const std::string &HeaderName,const std::string  &Value);
        void _BuildBasicVar(std::vector <std::string> &Var);
        bool _childeProcesse(char **ENV, char **ARG, int pip[2]);
        int _ParentProcesse(char **ENV, char **ARG, int pip[2]);
        bool _InintialVar(char **ENV, char **ARG, int pip[2]);
        char **_StoredArgs();
        char **_MakeEnv();
    public:
        clsCGI();
        bool GetIsRunCGI();
        clsParseOutCGI &GetclsParseOutCGI();
        int RunCGI();
        void SetIsRunCGI(bool IsRunCGI);
        ~clsCGI();
};

#endif