/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:15:57 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 16:54:34 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>

void serverKill(int);
#include <iostream>
#include <cstdlib>
#include <csignal>
#include "WebServer.hpp"
#include "utils/Utils.hpp"
#include "conf/Config.hpp"
#include "conf/Server.hpp"
#include "log/Log.h"

WebServer *g_server = NULL;
std::vector<Config> config;
std::vector<Server> pureServers;

static void ignore_signal(int sig) { signal(sig, SIG_IGN); }

static void setup_signal_handlers()
{
    signal(SIGINT, serverKill);
    ignore_signal(SIGPIPE);
    ignore_signal(SIGTSTP);
    ignore_signal(SIGQUIT);
}

static void cleanup_and_exit(int status)
{
    delete g_server;
    Log::info("Servers killed");
    ::exit(status);
}

static void print_no_server_error()
{
    std::cout << "\033[31m";
    std::cout << "[ERROR] " << Utils::getTime() << ": " << "No server found in the configuration file."
              << "\033[0m" << std::endl;
}

static bool build_config_from_file(const char* filename)
{
    Log::info("Parsing configuration file.");
    pureServers = Config::parse(filename);

    if (pureServers.size() == 0)
    {
        print_no_server_error();
        return false;
    }

    config = Config::set(pureServers);

    if (config.empty())
    {
        std::cerr << "webserv: invalid configuration - conflicting listen ports detected." << std::endl;
        return false;
    }

    Log::info("Configuration file parsed");
    return true;
}

static void init_and_launch_server()
{
    g_server = new WebServer(config);
    setup_signal_handlers();
    g_server->start();
}

void serverKill(int sig)
{
    (void)sig;
    (void)pureServers.empty();
    (void)config.empty();
    cleanup_and_exit(1);
}

void signalSetter()
{
    setup_signal_handlers();
}

int main(int ac, char **av)
{
    if (ac == 2)
    {
        try
        {
            if (!build_config_from_file(av[1]))
                return 1;

            init_and_launch_server();
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
            return 1;
        }
        return 0;
    }

    std::cerr << "Please try : ./webserv [configuration file]" << std::endl;
    return 1;
}

