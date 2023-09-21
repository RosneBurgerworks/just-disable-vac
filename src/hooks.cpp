/*
 * hooks.cpp
 *
 *  Created on: September 20, 2023
 *      Author: rosne-gamingyt
 */

#include <iostream>
#include <dlfcn.h>
#include <ftw.h>
#include <cstring>
#include <cstdlib>
#include "log.hpp"
#include "header.hpp"
#include "fakedev.hpp"

/*
 * ORIGINAL FUNCTIONS TYPES
 */

typedef FILE* (*fopen_fn)(const char*, const char*);
typedef char* (*fgets_fn)(char*, int, FILE*);
typedef int (*fclose_fn)(FILE*);
typedef int (*nftw_fn)(const char*, int (*)(const char*, const struct stat*, int, struct FTW*), int, int);

/*
 * ORIGINAL FUNCTIONS HANDLES
 */

nftw_fn HIDDEN get_nftw()
{
    return reinterpret_cast<nftw_fn>(dlsym(RTLD_NEXT, "nftw"));
}

fgets_fn HIDDEN get_fgets()
{
    return reinterpret_cast<fgets_fn>(dlsym(RTLD_NEXT, "fgets"));
}

#if LOG
fopen_fn HIDDEN get_fopen()
{
    return reinterpret_cast<fopen_fn>(dlsym(RTLD_NEXT, "fopen"));
}

fclose_fn HIDDEN get_fclose()
{
    return reinterpret_cast<fclose_fn>(dlsym(RTLD_NEXT, "fclose"));
}
#endif

/*
 * HOOKED FUNCTIONS
 */

/*
 *  "/proc/%d/status" "rb"
 *  "/proc/%u/maps" "rb"
 */

#if LOG
FILE* fopen(const char* filename, const char* mode)
{
    FILE* retval = get_fopen()(filename, mode);
    log(LOG_DEBUG, "fopen %s %08x '%s'", mode, reinterpret_cast<unsigned int>(retval), filename);
    if (strstr(filename, "/sys/devices"))
        log(LOG_WARNING, "fopen /sys/devices is accessed!");
    return retval;
}
#endif

/*
 *  /sys/devices
 */
int EXPORT nftw(const char* path, int (*fn)(const char*, const struct stat*, int, struct FTW*), int maxfds, int flags)
{
    log(LOG_INFO, "nftw %s", path);
    if (!std::strcmp(path, "/sys/devices") || !std::strcmp(path, "/sys/devices/"))
    {
        if (fakedev() != 0)
        {
            log(LOG_INFO, "devinfo success");
            return get_nftw()(fakedev(), fn, maxfds, flags);
        }
        else
        {
            log(LOG_ERR, "devinfo fail");
        }
    }
    return get_nftw()(path, fn, maxfds, flags);
}

/*
 *  "TracerPid:\t%d"
 *  cathook
 */
EXPORT char* fgets(char* s, int n, FILE* stream)
{
    char* buffer = reinterpret_cast<char*>(std::malloc(n));
    char* retval = get_fgets()(buffer, n, stream);

    if (retval == buffer)
    {
        retval = s;

        int tracerPID = 0;

        if (std::sscanf(buffer, "TracerPid:\t%d", &tracerPID) == 1)
        {
            log(LOG_WARNING, "tracer (%d)", tracerPID);
            std::strcpy(buffer, "TracerPid:\t0\n");
        }

        while (strstr(buffer, "cathook") || strstr(buffer, "libvpcfs.so"))
        {
            log(LOG_WARNING, "intercepted '%s'", buffer);
            retval = get_fgets()(buffer, n, stream);
            if (std::sscanf(buffer, "TracerPid:\t%d", &tracerPID) == 1)
            {
                log(LOG_WARNING, "tracer (%d)", tracerPID);
                std::strcpy(buffer, "TracerPid:\t0\n");
            }
            if (retval == buffer)
                retval = s;
            else
                return nullptr;
        }

    }

    std::memcpy(s, buffer, n);
    std::free(buffer);
    return retval;
}

#if LOG
int fclose(FILE* stream)
{
    log("fclose %08x", reinterpret_cast<unsigned int>(stream));
    return get_fclose()(stream);
}
#endif