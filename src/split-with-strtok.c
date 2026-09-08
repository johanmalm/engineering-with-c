#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s <string> <delim>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

//! [0]
	char *saveptr, *token, *str = argv[1], *delim = argv[2];
	while ((token = strtok_r(str, delim, &saveptr))) {
		printf("%s\n", token);
		str = NULL;
	}
//! [0]

	exit(EXIT_SUCCESS);
}
