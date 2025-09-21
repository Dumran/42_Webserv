#pragma once

#include <string>
#include "Client.hpp"

class Log
{
public:
    static void info(const std::string &msg);
    static void response(Client &client);

};
