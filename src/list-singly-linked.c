// SPDX-License-Identifier: MIT
#include <stdio.h>
#include <stdlib.h>
#include "macros.h"
#include "util.h"

//! [0]
struct item {
	int value;
	struct item *next;
};
//! [0]

//! [1]
static void list_add(struct item **head, int value)
{
	struct item *item = xcalloc(1, sizeof(*item));
	item->value = value;
	item->next = *head;
	*head = item;
}
//! [1]

static void list_remove_item_by_value(struct item **head, int to_remove)
{
	struct item **pp = head;
	struct item *entry;

	while (*pp) {
		entry = *pp;
		if (entry->value == to_remove) {
			*pp = entry->next;
			free(entry);
		} else {
			pp = &entry->next;
		}
	}
}

static void list_destroy(struct item **head)
{
	struct item **pp = head;
	while (*pp) {
		struct item *entry = *pp;
		*pp = entry->next;
		free(entry);
	}
}

int main(int argc, char **argv)
{
//! [2]
	struct item *items = NULL;

	const int ints[] = { 4, 3, 2, 1, 0 };
	for (size_t i = 0; i < ARRAY_SIZE(ints); ++i) {
		list_add(&items, ints[i]);
	}
//! [2]

	list_remove_item_by_value(&items, 2);

//! [3]
	for (struct item *p = items; p; p = p->next) {
		printf("%d\n", p->value);
	}
//! [3]

	list_destroy(&items);
}

