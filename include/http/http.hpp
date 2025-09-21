#pragma once

#include <string>
#include "HttpMethod.hpp"
#include "HttpStatus.hpp"

#define MAX_EVENTS 10
#define HOME_DIR "www/"

enum RequestType {
    NONE = 0,
    REQUEST = 1,
    WAIT_FORM = 2,
    EXITED = 3
};
