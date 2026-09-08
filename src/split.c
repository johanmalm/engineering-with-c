// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include "string-split.h"

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s <string> <delimiter>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char **parts = string_split(argv[1], argv[2]);
	for (char **p = parts; *p; p++) {
		printf("%s\n", *p);
	}
	argv_free(parts);
}
