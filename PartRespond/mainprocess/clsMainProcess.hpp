/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:40:27 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/17 17:37:43 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_MAIN_PROCESS_HPP
# define CLS_MAIN_PROCESS_HPP

#include "../response/clsResponse.hpp"
#include "../cgi/clsParseOutCGI.hpp"
#include "../cgi/clsCGI.hpp"

class clsMainProcess
{
    clsResponse Response;
    clsParseOutCGI ParseOutCGI;
    clsCGI CGI;
    int _Status;
    std::map <short,short> _Mod;
    public:
        clsMainProcess();
        clsResponse GetclsResponse();
        void PartRedirection();
        void PartPermission();
        void PartCGI();
        void PartDeleteMethod();
        void PartPOSMethod();
        void PartGETMethod();
        void MainProcess();
        ~clsMainProcess();
};
#endif

