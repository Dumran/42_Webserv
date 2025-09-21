#pragma once

#include <string>
#include <map>

class HttpStatus
{
private:
static const std::map<int, std::string> status_text;

public:
    static const int NONE = -1;
    static const int OK = 200;
    static const int CREATED = 201;
    static const int MOVED_PERMANENTLY = 301;
    static const int FOUND = 302;
    static const int NOT_MODIFIED = 304;
    static const int TEMPORARY_REDIRECT = 307;
    static const int PERMANENT_REDIRECT = 308;
    static const int BAD_REQUEST = 400;
    static const int FORBIDDEN = 403;
    static const int NOT_FOUND = 404;
    static const int METHOD_NOT_ALLOWED = 405;
    static const int NOT_ACCEPTABLE = 406;
    static const int REQUEST_TIMEOUT = 408;
    static const int LENGTH_REQUIRED = 411;
    static const int PAYLOAD_TOO_LARGE = 413;
    static const int UNSUPPORTED_MEDIA_TYPE = 415;
    static const int INTERNAL_SERVER_ERROR = 500;

    static std::string to_string(int code);
    static std::map<int, std::string> make_status_text();
};
