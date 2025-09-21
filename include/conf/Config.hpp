/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:21 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:14:22 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"

class Config
{

private:
    Server _default;
    std::vector<Server> possible_servers;
    static bool check_braces(const std::string& filename);

public:
    static bool validate_ip(const std::string& ip);
    Config();
    ~Config();
    
    Server& getDefault();
    const std::vector<Server>& getPossibleServers() const;
    std::vector<Server>& getPossibleServers();
    
    void setDefault(const Server& server);
    void setPossibleServers(const std::vector<Server>& servers);
    void addServer(const Server& server);
    
    static std::vector<Server> parse(const std::string& filename);
    static std::vector<Config> set(std::vector<Server> &servers);
    static std::string trim_space(const std::string &s);

};
