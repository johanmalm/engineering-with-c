#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "macros.h"
#include "util.h"

struct item {
	int value;
	struct list_head link;
};

//! [0]
static void list_reverse(struct list_head *head)
{
	struct list_head *curr = head;
	do {
		struct list_head *tmp = curr->next;
		curr->next = curr->prev;
		curr->prev = tmp;
		curr = curr->prev;
	} while (curr != head);
}
//! [0]

static void add_item(struct list_head *items, int value)
{
	struct item *item = xcalloc(1, sizeof(*item));
	item->value = value;
	list_add_tail(&item->link, items);
}

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
	struct list_head items = LIST_HEAD_INIT(items);

	const int ints[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	for (size_t i = 0; i < ARRAY_SIZE(ints); ++i) {
		add_item(&items, ints[i]);
	}

//! [1]
	list_reverse(&items);
//! [1]

	struct item *item;
	list_for_each_entry(item, &items, link) {
		printf("%d", item->value);
	}
	printf("\n");

	list_destroy(&items);
}
