/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   librarys.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achamdao <achamdao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 16:01:35 by achamdao          #+#    #+#             */
/*   Updated: 2026/02/28 17:26:31 by achamdao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef LIBRARYS_HPP
# define LIBRARYS_HPP

enum eSatat {UPLOAD = -5, REDIRECTION, CHUNK, CGI};

struct stMod
{
    enum eMod {UPLOAD , REDIRECTION, CHUNK, CGI, ERROR};
};

#include "../../Utils/HelperFunctions.hpp"

#endif

