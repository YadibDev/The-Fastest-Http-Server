/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsCGI.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 14:39:09 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/17 17:36:34 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_CGI_HPP
# define CLS_CGI_HPP

#include "../mainprocess/librarys.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"

class clsCGI
{
    private:
        pid_t _PIDCHILD;
        bool _IsRunCGI;
        long long _StartTime;
        int _FD;
        RequestHandler _DataRequest;
        std::map<std::string, bool> _WhiteBlakHeaders;
        void _StoredWhiteBlakHeaders();
        std::string _BuildVarEnv(std::string &HeaderName,const std::vector<std::string>  &Value);
    public:
        clsCGI();
        long long GetCurrentTime();
        char **MakeEnv();
        int RunCGI();
        ~clsCGI();
};

#endif