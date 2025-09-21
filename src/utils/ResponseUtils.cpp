#include "utils/ResponseUtils.hpp"
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>

// std::string ResponseUtils::extractFileName(std::string request)
// {
//     size_t pos = request.find(" ");
//     if (pos == std::string::npos)
//     {
//         return "";
//     }

//     size_t start = pos + 1;
//     size_t end = request.find(" ", start);
//     if (end == std::string::npos)
//     {
//         return "";
//     }

//     std::string path = request.substr(start, end - start);
//     if (path == "/")
//         return path;

//     if (isDirectory(path))
//         return path;
//     return path;
// }

std::string ResponseUtils::extractFileName(std::string request)
{
    size_t space1 = request.find(" ");
    if (space1 == std::string::npos)
        return std::string("");

    size_t space2 = request.find(" ", space1 + 1);
    if (std::string::npos == space2)
        return "";

    size_t length = space2 - space1 - 1;
    std::string extractedPath = request.substr(space1 + 1, length);

    if ("/" == extractedPath)
        return extractedPath;

    if (isDirectory(extractedPath) != false)
        return extractedPath;

    return extractedPath;
}


bool ResponseUtils::isDirectory(const std::string &path)
{
    struct stat s;
    if (stat(path.c_str(), &s) == 0)
    {
        return S_ISDIR(s.st_mode);
    }
    return false;
}

std::string ResponseUtils::getContentType(const std::string &http_buffer)
{
    std::istringstream stream(http_buffer);
    std::string line;
    while (std::getline(stream, line))
    {
        size_t pos = line.find("Content-Type:");
        if (pos != std::string::npos)
        {
            return line.substr(pos + 14);
        }
    }
    return "";
}

size_t ResponseUtils::utilGetContentLength(std::string request, Response &response)
{
    size_t contentLength = 0;

    size_t pos = request.find("Content-Length:");
    if (pos != std::string::npos)
    {
        std::istringstream iss(request.substr(pos));
        std::string temp;
        iss >> temp >> contentLength;
    }
    if (request.find("Transfer-Encoding: chunked") != std::string::npos)
    {
        response.setIsChunked(true);
        contentLength = 0;
    }
    return (contentLength);
}

