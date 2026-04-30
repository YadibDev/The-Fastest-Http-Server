/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   librarys.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yadib <yadib@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 16:01:35 by achamdao          #+#    #+#             */
/*   Updated: 2026/04/29 14:53:28 by yadib            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef LIBRARYS_HPP
# define LIBRARYS_HPP


# define MAX_HEADERS  2000
# define MAX_BODY  SIZE_BUFFER - MAX_HEADERS

enum eSatat {UPLOAD = -5, REDIRECTION, CHUNK, CGI};

struct stMod
{
    enum eMod {EMPTY, UPLOAD , GET, POST, DELETE, REDIRECTION, CHUNK, ERROR, INTERNALRE, NOTINTERNALRE};
};

#include "../../Utils/HelperFunctions.hpp"

#endif

