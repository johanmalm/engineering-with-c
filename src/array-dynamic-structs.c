// SPDX-License-Identifier: MIT
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string-helpers.h"
#include "util.h"

struct item {
	char *name;
};

//! [0]
static struct item *items;
static size_t nr_items, alloc_items;
//! [0]

//! [1]
static struct item *add_item(void)
{
	if (nr_items == alloc_items) {
		alloc_items = (alloc_items + 16) * 2;
		items = xrealloc(items, alloc_items * sizeof(struct item));
	}
	struct item *item = items + nr_items;
	memset(item, 0, sizeof(*item));
	nr_items++;
	return item;
}
//! [1]

//! [2]
static void items_append(const char *name)
{
	assert(name);
	struct item *item = add_item();
	item->name = xstrdup(name);
}
//! [2]

static void items_destroy(void)
{
	for (size_t i = 0; i < nr_items; i++) {
		struct item *item = &items[i];
		free(item->name);
	}
	free(items);
}

/*
 * Note: 'void' refers to a pointer whose type information has been deliberately
 * discarded.
 */
//! [4]
static int compare_item_name(const void *a, const void *b)
{
	const struct item *item_a = a;
	const struct item *item_b = b;
	return strcmp0(item_a->name, item_b->name);
}
//! [4]

int main(int argc, char **argv)
{
//! [3]
	items_append("Foo");
	items_append("Bar");
	items_append("Baz");
	qsort(items, nr_items, sizeof(struct item), compare_item_name);
//! [3]

//! [5]
	for (size_t i = 0; i < nr_items; i++) {
		struct item *item = &items[i];
		printf("%s\n", item->name);
	}
//! [5]

	items_destroy();
}
