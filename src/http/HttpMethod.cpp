/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:20:48 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:20:49 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/HttpMethod.hpp"

const std::string HttpMethod::names[3] = {"GET", "POST", "DELETE"};

std::string HttpMethod::to_string(int method)
{
    switch (method)
    {
    case GET:
        return "GET";
    case POST:
        return "POST";
    case DELETE:
        return "DELETE";
    default:
        return "UNKNOWN";
    }
}
