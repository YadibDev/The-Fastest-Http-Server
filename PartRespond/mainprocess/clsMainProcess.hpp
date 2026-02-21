/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clsMainProcess.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 15:40:27 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/20 17:43:17 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLS_MAIN_PROCESS_HPP
# define CLS_MAIN_PROCESS_HPP

#include "../response/clsResponse.hpp"
#include "../cgi/clsParseOutCGI.hpp"
#include "../cgi/clsCGI.hpp"
#include "../../Parser/RequestHandler/RequestHandler.hpp"

class clsMainProcess
{
    private:
        clsResponse _Response;
        // clsParseOutCGI _ParseOutCGI;
        // clsCGI _CGI;
        RequestHandler _DataRequest;
        void _PartRedirection();
        void _PartPermission();
        void _PartCGI();
        void _PartDeleteMethod();
        void _PartPOSMethod();
        void _PartGETMethod();
    public:
        void MainProcess(const RequestHandler &DataRequest);
        clsMainProcess();
        clsResponse GetclsResponse(); 
        ~clsMainProcess();
};
#endif

