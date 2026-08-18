// SPDX-License-Identifier: MIT
#include <stdio.h>
#include <stdlib.h>
#include "string-helpers.h"

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("usage: %s <string>\n", argv[0]);
		return EXIT_FAILURE;
	}
	remove_spaces(argv[1]);
	printf("%s\n", argv[1]);
}
