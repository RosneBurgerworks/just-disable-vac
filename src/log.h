/*
 * log.h
 *
 *  Created on: Aug 11, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <syslog.h>
#include "header.h"

#if LOG

#define log(level, ...) \
    do { \
        openlog("libvpcfs", LOG_PID, LOG_USER); \
        syslog(level, __VA_ARGS__); \
        closelog(); \
        printf(__VA_ARGS__); \
        printf("\n"); \
    } while(0)

#else

#define log(...)    do { } while(0)

#endif