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
