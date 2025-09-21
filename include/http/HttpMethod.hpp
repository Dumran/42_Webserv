/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:34 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:14:35 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class HttpMethod
{
public:
    static const int GET = 2147483647;
    static const int POST = 2147483646;
    static const int DELETE = 2147483645;

    static const std::string names[3];

    static std::string to_string(int method);
};
