/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:12 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:30:08 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <cerrno>
#include "conf/Config.hpp"
#include "Client.hpp"
#include <poll.h>

class WebServer
{
private:
    std::vector<Config> &configs;
    std::vector<pollfd> poll_fds;
    std::vector<Client> clients;
    int server_size;
    Server server;

    void serverResponse(Client &client);
    void setNonBlocking(int fd);
    int socketCreator(Server &server);
    bool checkResponse(Client &client, std::string &headers);
    void addClient(int fd, short events, size_t i);
    void closeClient(int index);
    void cgiHandle(Client &client);
    void readFormData(int i);
    int newConnection(size_t i);
    void serversCreator(std::vector<Config> &configs);
    void clientSend(int i);

public:
    WebServer(std::vector<Config> &configs);
    ~WebServer();

    void start();

    int wait_for_events();
    bool is_server_index(int) const;
    bool is_client_index(int) const;
    void handle_error_or_hup(int);
    void handle_server_readable(int);
    void arm_write_if_needed(int, const Client&);
    void handle_client_readable(int, Client&);
    void handle_client_writable(int, Client&, short);
    void process_fd(int);

    class ServerExcp : public std::exception
    {
    private:
        std::string excp;

    public:
        virtual const char *what() const throw();
        ServerExcp(const std::string &);
        ~ServerExcp() throw();
    };
};
