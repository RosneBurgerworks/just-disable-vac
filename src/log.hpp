/*
 * log.hpp
 *
 *  Created on: September 20, 2023
 *      Author: rosne-gamingyt
 */

#pragma once

#include <syslog.h>
#include <cstdio>
#include "header.hpp"

#if LOG

#define log(level, ...) \
    do { \
        openlog("libvpcfs", LOG_PID, LOG_USER); \
        syslog(level, __VA_ARGS__); \
        closelog(); \
        std::printf(__VA_ARGS__); \
        std::printf("\n"); \
    } while(0)

#else

#define log(...)    do { } while(0)

#endif