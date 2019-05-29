/*
 * hooks.c
 *
 *  Created on: Aug 10, 2017
 *      Author: nullifiedcat
 */

#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <ftw.h>
#include <string.h>
#include <malloc.h>

#include "log.h"
#include "header.h"
#include "fakedev.h"

#ifndef RTLD_NEXT
#	define RTLD_NEXT -1
#endif

/*
 * ORIGINAL FUNCTIONS TYPES
 */

typedef FILE *(*fopen_fn)(const char *, const char *);
typedef char *(*fgets_fn)(char *, int, FILE *);
typedef int (*fclose_fn)(FILE *);
typedef int (*nftw_fn)(const char *, int (*)(const char *, const struct stat *, int, struct FTW *), int, int);

/*
 * ORIGINAL FUNCTIONS HANDLES
 */

nftw_fn
HIDDEN get_nftw() {
	return dlsym(RTLD_NEXT, "nftw");
}

fgets_fn
HIDDEN get_fgets() {
	return dlsym(RTLD_NEXT, "fgets");
}

#if LOG
fopen_fn
HIDDEN get_fopen() {
	return dlsym(RTLD_NEXT, "fopen");
}

fclose_fn
HIDDEN get_fclose() {
	return dlsym(RTLD_NEXT, "fclose");
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
FILE *
fopen(const char *filename, const char *mode) {
	FILE *retval = get_fopen()(filename, mode);
	log(LOG_DEBUG, "fopen %s %08x '%s'", mode, retval, filename);
	if (strstr(filename, "/sys/devices")) {
		log(LOG_WARNING, "fopen /sys/devices is accessed!");
	}
	return retval;
}
#endif

/*
 *  /sys/devices
 */
int
EXPORT nftw(const char *path,
		 int (*fn)(const char *, const struct stat *, int, struct FTW *),
		 int maxfds, int flags) {

	log(LOG_INFO, "nftw %s", path);
	if (!strcmp(path, "/sys/devices") || !strcmp(path, "/sys/devices/")) {
		if (0 != fakedev()) {
			log(LOG_INFO, "devinfo success");
			return get_nftw()(fakedev(), fn, maxfds, flags);
		} else {
			log(LOG_ERR, "devinfo fail");
		}
	}
	return get_nftw()(path, fn, maxfds, flags);
}

/*
 *	"TracerPid:\t%d"
 *	cathook
 */
EXPORT char *
fgets(char *s, int n, FILE *stream) {
	char *buffer = malloc(n);
	char *retval = get_fgets()(buffer, n, stream);

	if (retval == buffer) {
		retval = s;

		int tracerPID = 0;

		if (sscanf(buffer, "TracerPid:\t%d", &tracerPID)) {
			log(LOG_WARNING, "tracer (%d)", tracerPID);
			strcpy(buffer, "TracerPid:\t0\n");
		}

		while (strstr(buffer, "cathook") || strstr(buffer, "libvpcfs.so")) {
			log(LOG_WARNING, "intercepted '%s'", buffer);
			retval = get_fgets()(buffer, n, stream);
			if (sscanf(buffer, "TracerPid:\t%d", &tracerPID)) {
				log(LOG_WARNING, "tracer (%d)", tracerPID);
				strcpy(buffer, "TracerPid:\t0\n");
			}
			if (retval == buffer) {
				retval = s;
			} else {
				return 0;
			}
		}

	}

	memcpy(s, buffer, n);
	free(buffer);
	return retval;
}

#if LOG
/*int
fclose(FILE *stream) {
	log("fclose %08x", stream);
	return get_fclose()(stream);
}*/
#endif
