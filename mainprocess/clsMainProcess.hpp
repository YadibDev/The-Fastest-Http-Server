/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:40:27 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/17 15:25:38 by achamdao         ###   ########.fr       */
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
    Data Data;
    int _Status;
    std::map <short,short> _Mod;
    public:
        clsResponse GetclsResponse();
        void PartRedirection();
        void PartPermission();
        void PartCGI();
        void PartDeleteMethod();
        void PartPOSMethod();
        void PartGETMethod();
        void MainProcess();
};
#endif

