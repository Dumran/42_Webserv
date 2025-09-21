#include "http/HttpStatus.hpp"

const int HttpStatus::NONE;
const int HttpStatus::OK;
const int HttpStatus::CREATED;
const int HttpStatus::MOVED_PERMANENTLY;
const int HttpStatus::FOUND;
const int HttpStatus::NOT_MODIFIED;
const int HttpStatus::TEMPORARY_REDIRECT;
const int HttpStatus::PERMANENT_REDIRECT;
const int HttpStatus::BAD_REQUEST;
const int HttpStatus::FORBIDDEN;
const int HttpStatus::NOT_FOUND;
const int HttpStatus::METHOD_NOT_ALLOWED;
const int HttpStatus::NOT_ACCEPTABLE;
const int HttpStatus::REQUEST_TIMEOUT;
const int HttpStatus::LENGTH_REQUIRED;
const int HttpStatus::PAYLOAD_TOO_LARGE;
const int HttpStatus::UNSUPPORTED_MEDIA_TYPE;
const int HttpStatus::INTERNAL_SERVER_ERROR;

std::map<int, std::string> HttpStatus::make_status_text()
{
    std::map<int, std::string> m;
    m.insert(std::make_pair(HttpStatus::NONE, "None"));
    m.insert(std::make_pair(HttpStatus::OK, "OK"));
    m.insert(std::make_pair(HttpStatus::CREATED, "Created"));
    m.insert(std::make_pair(HttpStatus::MOVED_PERMANENTLY, "Moved Permanently"));
    m.insert(std::make_pair(HttpStatus::FOUND, "Found"));
    m.insert(std::make_pair(HttpStatus::NOT_MODIFIED, "Not Modified"));
    m.insert(std::make_pair(HttpStatus::TEMPORARY_REDIRECT, "Temporary Redirect"));
    m.insert(std::make_pair(HttpStatus::PERMANENT_REDIRECT, "Permanent Redirect"));
    m.insert(std::make_pair(HttpStatus::BAD_REQUEST, "Bad Request"));
    m.insert(std::make_pair(HttpStatus::FORBIDDEN, "Forbidden"));
    m.insert(std::make_pair(HttpStatus::NOT_FOUND, "Not Found"));
    m.insert(std::make_pair(HttpStatus::METHOD_NOT_ALLOWED, "Method Not Allowed"));
    m.insert(std::make_pair(HttpStatus::NOT_ACCEPTABLE, "Not Acceptable"));
    m.insert(std::make_pair(HttpStatus::REQUEST_TIMEOUT, "Request Timeout"));
    m.insert(std::make_pair(HttpStatus::LENGTH_REQUIRED, "Length Required"));
    m.insert(std::make_pair(HttpStatus::PAYLOAD_TOO_LARGE, "Payload Too Large"));
    m.insert(std::make_pair(HttpStatus::UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"));
    m.insert(std::make_pair(HttpStatus::INTERNAL_SERVER_ERROR, "Internal Server Error"));
    return m;
}

const std::map<int, std::string> HttpStatus::status_text = HttpStatus::make_status_text();

std::string HttpStatus::to_string(int code)
{
    std::map<int, std::string>::const_iterator it = status_text.find(code);
    if (it != status_text.end())
    {
        return it->second;
    }
    return "Unknown Status";
}
