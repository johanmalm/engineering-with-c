#include <stdio.h>
#include "macros.h"

int main(void)
{
//! [0]
	const struct key_combos {
		const char *binding;
		const char *action;
	} key_combos[] = { {
			.binding = "A-Tab",
			.action = "NextWindow",
		}, {
			.binding = "A-S-Tab",
			.action = "PreviousWindow"
		},
	};

	for (size_t i = 0; i < ARRAY_SIZE(key_combos); i++) {
		printf("%s: %s\n", key_combos[i].binding, key_combos[i].action);
	}
//! [0]
}
