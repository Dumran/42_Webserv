#include "http/HttpMethod.hpp"

const std::string HttpMethod::names[3] = {"GET", "POST", "DELETE"};

std::string HttpMethod::to_string(int method)
{
    switch (method)
    {
    case GET:
        return "GET";
    case POST:
        return "POST";
    case DELETE:
        return "DELETE";
    default:
        return "UNKNOWN";
    }
}
