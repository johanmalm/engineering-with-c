#include <stdio.h>
#include <stdlib.h>
#include "cleanup.h"
#include "util.h"

//! [0]
int main(void) {
	cleanup_free char *buffer = xcalloc(1, 4096);
	/* Do something with buffer */
}
//! [0]
