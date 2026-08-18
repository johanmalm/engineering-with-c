// SPDX-License-Identifier: MIT
#include "parse-key-value-pair.h"
#include <assert.h>
#include <string.h>
#include "string-helpers.h"

void parse_key_value_pair(char *line, char **key, char **value, char delim)
{
	assert(line);
	char *p = line;
	while ((p[0] == ' ') || (p[0] == '\t')) {
		p++;
	}
	if (p[0] == '#') {
		return;
	}
	p = strchr(line, delim);
	if (!p) {
		return;
	}
	p[0] = '\0';
	*key = string_strip(line);
	*value  = string_strip(++p);
}
