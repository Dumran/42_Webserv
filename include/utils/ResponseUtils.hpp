/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseUtils.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:48 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:14:49 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include "http/http.hpp"
#include "Response.hpp"

class ResponseUtils
{
public:
    static std::string extractFileName(std::string request);
    static bool isDirectory(const std::string& path);
    static std::string getContentType(const std::string& http_buffer);
    static size_t utilGetContentLength(std::string request, Response &response);
};
