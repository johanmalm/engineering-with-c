#include <stdlib.h>
#include "util.h"

//! [0]
int main(void) {
	char *buffer = xmalloc(4096);
	/* Do something with buffer */
	free(buffer);
}
//! [0]
