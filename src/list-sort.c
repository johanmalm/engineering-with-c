#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "list_sort.h"
#include "macros.h"
#include "util.h"

//! [1]
struct item {
	int value;
	struct list_head link;
};
//! [1]

//! [6]
static int compare_ints(void *priv, const struct list_head *a, const struct list_head *b)
{
	(void)priv;
	struct item *item_a = container_of(a, struct item, link);
	struct item *item_b = container_of(b, struct item, link);
	if (item_a->value < item_b->value) {
		return -1;
	}
	if (item_a->value > item_b->value) {
		return 1;
	}
	return 0;
}
//! [6]

//! [2]
static void add_item(struct list_head *items, int value)
{
	struct item *item = xcalloc(1, sizeof(*item));
	item->value = value;
	list_add_tail(&item->link, items);
}
//! [2]

static void list_destroy(struct list_head *items)
{
	struct item *item, *next;
	list_for_each_entry_safe(item, next, items, link) {
		list_del(&item->link);
		free(item);
		item = NULL;
	}
}

int main(void)
{
//! [0]
	LIST_HEAD(items);
//! [0]

//! [3]
	const int ints[] = { 4, 2, 9, 1, 7, 0, 5, 3, 8, 6 };
	for (size_t i = 0; i < ARRAY_SIZE(ints); ++i) {
		add_item(&items, ints[i]);
	}
//! [3]

	/* Use the kernel merge-sort routine */
//! [5]
	list_sort(NULL, &items, compare_ints);
//! [5]

//! [4]
	struct item *item;
	list_for_each_entry(item, &items, link) {
		printf("%d", item->value);
	}
//! [4]
	printf("\n");

	list_destroy(&items);
}
