/*
 * hooks.h
 *
 *  Created on: Aug 11, 2017
 *      Author: nullifiedcat
 */

#pragma once

#include <stdio.h>

#include "header.h"

int EXPORT nftw(const char* path, int (* fn)(const char*, const struct stat*, int, struct FTW*), int maxfds, int flags);

char* EXPORT fgets(char* s, int n, FILE* stream);

#if LOG
int EXPORT fclose(FILE* stream);

FILE* EXPORT fopen(const char* filename, const char* mode);
#endif
