#include <stdio.h>
#include <stdlib.h>
#include "util.h"

//! [0]
void freep(char **ptr) {
	free(*ptr);
}
//! [0]

//! [1]
int main(void) {
	__attribute__((__cleanup__(freep))) char *buffer = xcalloc(1, 4096);
	/* Do something with buffer */
}
//! [1]
