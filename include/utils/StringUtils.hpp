#pragma once

#include <string>
#include <vector>

class StringUtils
{
public:
    static std::vector<std::string> split(const std::string &str, char delimiter);
    static std::string from(int num);
};
