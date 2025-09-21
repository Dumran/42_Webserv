/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehkekli <mehkekli@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 15:14:32 by mehkekli          #+#    #+#             */
/*   Updated: 2025/09/21 15:14:33 by mehkekli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
