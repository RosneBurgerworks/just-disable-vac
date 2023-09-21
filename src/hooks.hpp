/*
 * hooks.hpp
 *
 *  Created on: September 20, 2023
 *      Author: rosne-gamingyt
 */

#pragma once

#include <cstdio>

#include "header.hpp"

// Define struct FTW if not already defined
#ifndef _FTW
struct FTW {
    int base;
    int level;
};
#endif

extern int EXPORT nftw(const char* path, int (*fn)(const char*, const struct stat*, int, struct FTW*), int maxfds, int flags);

extern char* EXPORT fgets(char* s, int n, FILE* stream);

#if LOG
extern int EXPORT fclose(FILE* stream);

extern FILE* EXPORT fopen(const char* filename, const char* mode);
#endif