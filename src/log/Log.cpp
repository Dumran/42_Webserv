/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:21:00 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:21:01 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <iomanip>
#include <climits>
#include "log/Log.h"
#include "utils/Utils.hpp"
#include "Response.hpp"

void Log::info(const std::string &msg)
{
    std::cout << "\033[0;32m" << "[INFO] " << Utils::getTime() << ": " << msg << "\033[0m" << std::endl;
}

void Log::response(Client &client)
{
    Response &response = client.getResponse();
    if (response.getRequestType() == NONE)
        return;
    std::string meth(HttpMethod::names[INT_MAX - static_cast<int>(response.getRequestType())]);
    std::string servername = (client.getServerName().empty() != true) ? client.getServerName() : "0.0.0.0";

    std::cout << "\033[0;34m";
    std::cout << "[INFO] " << Utils::getTime() << ": Server=" << servername << " Method=<" << meth << "> URL=\""
              << response.getPureLink() << "\" Response=" << std::setw(2) << response.getResponseCode() << std::endl;
    std::cout << "\033[0m";
}
