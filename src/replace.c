// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include "string-split.h"

int main(int argc, char **argv)
{
	if (argc != 4) {
		fprintf(stderr, "usage: %s <string> <old> <new>>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char *result = string_replace(argv[1], argv[2], argv[3]);
	printf("%s\n", result);
	free(result);
}
