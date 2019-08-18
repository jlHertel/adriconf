//
// Created by jean on 17/08/19.
//

#ifndef ADRICONF_LOGGERLEVEL_H
#define ADRICONF_LOGGERLEVEL_H

#include <cstdint>

enum class LoggerLevel: std::int8_t {
    DEBUG = 1,
    INFO = 2,
    WARNING = 4,
    ERROR = 8
};


#endif //ADRICONF_LOGGERLEVEL_H
