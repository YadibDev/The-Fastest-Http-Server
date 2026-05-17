/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   librarys.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 16:01:35 by achamdao          #+#    #+#             */
/*   Updated: 2026/05/05 12:10:41 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef LIBRARYS_HPP
# define LIBRARYS_HPP


# define MAX_HEADERS  2000
# define MAX_BODY  SIZE_BUFFER - MAX_HEADERS

enum eSatat {UPLOAD = -5, REDIRECTION, CHUNK, CGI};

struct stMod
{
    enum eMod {EMPTY, UPLOAD , GET, POST, DELETE, REDIRECTION, CHUNK, ERROR, INTERNALRE, AUTOINDEX};
};

#include "../../Utils/HelperFunctions.hpp"

#endif

