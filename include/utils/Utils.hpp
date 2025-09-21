/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:55 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:14:56 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <iostream>
#include "Client.hpp"

class Utils
{
public:
    static bool wait(pid_t pid, int timeout_seconds);
    static std::string getTime();

    static std::string readFile(const std::string& fileName, Response& response, Client& client, int code);
    static bool readFileToString(const std::string& path, std::string& out);

    static void doubleSeparator(std::string key, std::string &buffer, Client &client);
    static int countSeparator(const std::string &buffer, const std::string &target);
    static std::string getHostHeader(const std::string& request);
    static void getServerByHost(const std::string &host, Client &client);
    static void parseChunked(Client &client, std::string &body);
    static std::string chunkedGetLine(std::istringstream &stream, int type);
    static void chunkedCompleted(Client &client, std::string &result);
    
    static std::string resToErrorPages(Response &response, int error_type, Client &client);
    static std::string getResponseHeader(Client &client);
    static void parseRsp(std::string &buffer, Client &client);
    static void getBufferFormData(std::string &buffer, Client &client);
    static void parseChunkedFt(Client &client, std::string &body, int type);
    static std::string generateAutoIndex(const std::string& path, const std::string& requestPath, Client &client);

};
