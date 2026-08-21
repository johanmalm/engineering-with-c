#include <stdio.h>
#include "macros.h"

int main(void)
{
//! [0]
	const char * const words[] = { "Foo", "Bar", "Baz" };

	for (size_t i = 0; i < ARRAY_SIZE(words); i++) {
		printf("%s\n", words[i]);
	}
//! [0]
}
