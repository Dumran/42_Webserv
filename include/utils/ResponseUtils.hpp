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
