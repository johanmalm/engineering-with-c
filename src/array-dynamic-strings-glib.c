#include "glib.h"
#include "stdio.h"

int main(void)
{
//! [0]
	g_autoptr(GPtrArray) items = g_ptr_array_new_with_free_func(g_free);

	g_ptr_array_add(items, g_strdup("Foo"));
	g_ptr_array_add(items, g_strdup("Bar"));
	g_ptr_array_add(items, g_strdup("Baz"));

	for (guint i = 0; i < items->len; ++i) {
		g_print("%s\n", (gchar *)g_ptr_array_index(items, i));
	}
//! [0]
}
