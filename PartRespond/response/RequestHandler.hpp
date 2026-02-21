/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 17:09:34 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/20 17:31:07 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <map>
#include <stack>
# ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP


struct stErrorPagedata
{
    short Status;
    std::string Path;
};
enum Method
{
    GET = 1 << 0,
    POST = 1 << 1,
    DELETE = 1 << 2
};

class RequestHandler
{
    
    public:
        std::string _physicalPath;
        bool        _allowMethod;
        std::string _query;
        Method        _method;
        std::map<std::string, std::vector<std::string> > _headers;
        std::map<int, stErrorPagedata> _error_pages;
        std::map<std::string, std::string>    _cgiInterpreter;
        std::string _body;
        std::string _filePathBody;
    public:
        bool            getAllowMethod() const;
        Method          getMethod() const;

        const std::string&  getPhysicalPath() const;
        const std::string&  getQuery() const;
        const std::string&  getBody() const;
        const std::string&  getFilePathBody() const;

        const std::map<std::string, std::vector<std::string> >& getHeaders() const;
        const std::map<int, stErrorPagedata>&                   getErrorPages() const;
        const std::map<std::string, std::string>&               getCgiInterpreter() const;
        int return_status;
        std::string return_url;
        std::string upload_store;
    
        RequestHandler();
};
# endif