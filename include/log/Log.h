/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:42 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:14:43 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "Client.hpp"

class Log
{
public:
    static void info(const std::string &msg);
    static void response(Client &client);

};
