#include "utils/StringUtils.hpp"
#include <sstream>

std::vector<std::string> StringUtils::split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    
    while (std::getline(token_stream, token, delimiter)) {
        if (token.empty())
            continue;
        tokens.push_back(token);
    }

    return tokens;
}

std::string StringUtils::from(int num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}
