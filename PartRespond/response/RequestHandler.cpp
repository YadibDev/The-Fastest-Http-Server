/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 17:17:52 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/19 15:18:29 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

 RequestHandler::RequestHandler(): _physicalPath(""), _autoindex(false),_allowMethod(0),
         return_status(0),return_url(""),upload_store("")
{
    
}
bool RequestHandler::getAutoindex() const {
    return _autoindex;
}

bool RequestHandler::getAllowMethod() const {
    return _allowMethod;
}

Method RequestHandler::getMethod() const {
    return _method;
}
const std::string& RequestHandler::getPhysicalPath() const {
    return _physicalPath;
}

const std::string& RequestHandler::getQuery() const {
    return _query;
}

const std::string& RequestHandler::getBody() const {
    return _body;
}

const std::string& RequestHandler::getFilePathBody() const {
    return _filePathBody;
}

const std::map<std::string, std::vector<std::string> >& RequestHandler::getHeaders() const {
    return _headers;
}

const std::map<int, stErrorPagedata>& RequestHandler::getErrorPages() const {
    return _error_pages;
}

const std::map<std::string, std::string>& RequestHandler::getCgiInterpreter() const {
    return _cgiInterpreter;
}