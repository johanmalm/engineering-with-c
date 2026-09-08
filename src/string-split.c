// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void argv_free(char **parts)
{
	if (parts) {
		for (char **p = parts; *p; p++) {
			free(*p);
		}
		free(parts);
	}
}

//! [0]
char **string_split(char *str, const char *delim)
{
	if (!str || !delim || !*delim) {
		return NULL;
	}

	size_t argc = 1;
	char *p = str;

	while ((p = strstr(p, delim))) {
		argc++;
		p += strlen(delim);
	}

	char **argv = xcalloc(argc + 1, sizeof(*argv));

	char **argvp = argv;
	char *start = str;
	size_t delim_len = strlen(delim);

	while ((p = strstr(start, delim))) {
		*argvp++ = strndup(start, p - start);
		start = p + delim_len;
	}

	*argvp++ = xstrdup(start);
	*argvp = NULL;

	return argv;
}
//! [0]

char *string_join(char **parts, const char *delim)
{
	if (!parts || !delim) {
		return NULL;
	}

	size_t delim_len = strlen(delim);
	size_t len = 1;

	for (char **p = parts; *p; p++) {
		len += strlen(*p);
		if (p[1]) {
			len += delim_len;
		}
	}

	char *str = xmalloc(len);
	char *dst = str;

	for (char **p = parts; *p; p++) {
		size_t n = strlen(*p);

		memcpy(dst, *p, n);
		dst += n;

		if (p[1]) {
			memcpy(dst, delim, delim_len);
			dst += delim_len;
		}
	}

	*dst = '\0';
	return str;
}

char *string_replace(char *str, const char *old, const char *new)
{
	char **parts = string_split(str, old);
	char *result = string_join(parts, new);
	argv_free(parts);
	return result;
}
