// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include "string-helpers.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>

//! [0]
void rtrim(char *s)
{
	assert(s);
	size_t len = strlen(s);
	if (!len) {
		return;
	}
	char *end = s + len - 1;
	while (end >= s && isspace(*end)) {
		end--;
	}
	*(end + 1) = '\0';
}
//! [0]

//! [1]
char *string_strip(char *s)
{
	assert(s);
	rtrim(s);
	while (isspace(*s)) {
		s++;
	}
	return s;
}
//! [1]

/* Copied from linux/tools/lib/strings.c under GPL-2.0-only */
//! [2]
void remove_spaces(char *s)
{
	assert(s);
	char *d = s;
	do {
		while (*d == ' ')
			++d;
	} while ((*s++ = *d++));
}
//! [2]

/* NULL-safe string comparison functions */
int strcmp0(const char *a, const char *b)
{
	if (!a && !b) {
		return 0;
	}
	if (!a) {
		return 1;
	}
	if (!b) {
		return -1;
	}
	return strcmp(a, b);
}

int strcasecmp0(const char *a, const char *b)
{
	if (!a && !b) {
		return 0;
	}
	if (!a) {
		return 1;
	}
	if (!b) {
		return -1;
	}
	return strcasecmp(a, b);
}
