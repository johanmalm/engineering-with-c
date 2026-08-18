// SPDX-License-Identifier: MIT
#define _POSIX_C_SOURCE 200809L
#include "util.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void info(const char *err, ...)
{
	va_list params;

	fprintf(stderr, "info: ");
	va_start(params, err);
	vfprintf(stderr, err, params);
	va_end(params);
	fprintf(stderr, "\n");
}

void warn(const char *err, ...)
{
	va_list params;

	fprintf(stderr, "warning: ");
	va_start(params, err);
	vfprintf(stderr, err, params);
	va_end(params);
	fprintf(stderr, "\n");
}

void die(const char *err, ...)
{
	va_list params;

	fputs("fatal: ", stderr);
	va_start(params, err);
	vfprintf(stderr, err, params);
	va_end(params);
	fputc('\n', stderr);

	exit(EXIT_FAILURE);
}

char *xstrdup(const char *s)
{
	if (!s) {
		return NULL;
	}
	char *ret = strdup(s);
	if (!ret) {
		die("strdup(): out of memory");
	}
	return ret;
}

void *xmalloc(size_t size)
{
	void *ret = malloc(size);
	if (!ret) {
		die("malloc(): out of memory");
	}
	return ret;
}

void *xrealloc(void *ptr, size_t size)
{
	void *ret = realloc(ptr, size);
	if (!ret) {
		die("realloc(): out of memory");
	}
	return ret;
}

void *xcalloc(size_t nb, size_t size)
{
	void *ret = calloc(nb, size);
	if (!ret) {
		die("calloc(): out of memory");
	}
	return ret;
}
