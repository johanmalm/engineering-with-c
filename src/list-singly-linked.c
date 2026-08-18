// SPDX-License-Identifier: MIT
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

struct item {
	int value;
	struct item *next;
};

static void list_insert_tail(struct item **head, int value)
{
	struct item *item = xcalloc(1, sizeof(*item));
	item->value = value;
	item->next = *head;
	*head = item;
}

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
	struct item *head = NULL;
	list_insert_tail(&head, 4);
	list_insert_tail(&head, 3);
	list_insert_tail(&head, 2);
	list_insert_tail(&head, 1);

	list_remove_item_by_value(&head, 2);

	for (struct item *p = head; p; p = p->next) {
		printf("%d\n", p->value);
	}

	list_destroy(&head);
}

