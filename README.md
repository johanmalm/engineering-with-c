# Engineering with C

<i>A collection of code snippets to demonstrate ways of working in C</i>

# Table of Contents

* [FOREWORD](#foreword)
* [STRINGS](#strings)
    * [Trim leading and trailing white-space from string](#trim-leading-and-trailing-white-space-from-string)
    * [Trim all white-space from string and truncate spaces](#trim-all-white-space-from-string-and-truncate-spaces)
* [ARRAYS](#arrays)
    * [Fixed-size arrays](#fixed-size-arrays)
    * [Dynamic arrays](#dynamic-arrays)

# FOREWORD

The primary goal of this book is to document methods of doing things in C. As a
secondary side-effect it also offers reflections on what programming in C
teaches us about Engineering.

It has been inspired by [pure-bash-bible] and [pure-sh-bible] in terms of the
types of code-snippets it includes.

Examples range from very simple, idiomatic approaches without helper libraries,
to more sophisticated methods using glib, talloc, cleanup attributes and the
like.

A small number of helpers are used to keep the examples brief and to the point;
for instance `xrealloc()` and `die()`.

Many example functions use `assert()` to set out what type of input is expected.
In terms of usage, these are only ever intended fail if the programmer has done
something wrong, not the user. So, depending on how you wish to use the code, it
may be better to replace the `assert()` with more graceful handling and an error
message.

In general, the code is intended to be portable but it has only be tested on
Linux, so feedback is appreciated if you find issues on other Operating Systems.

If you see something that is wrong, incorrectly described or missing I would be
grateful for a github issue or pull request.

[pure-bash-bible]: https://github.com/dylanaraps/pure-bash-bible
[pure-sh-bible]: https://github.com/dylanaraps/pure-sh-bible

# STRINGS

## Trim leading and trailing white-space from string

**Example Function:**

[string-helpers.c](src/string-helpers.c)

```
void rtrim(char *s)
{
	assert(s);
	size_t len = strlen(s);
	if (!len) {
		return;
	}
	char *end = s + len - 1;
	while (end >= s && isspace(*end)) {
		end--;
	}
	*(end + 1) = '\0';
}
```

```
char *string_strip(char *s)
{
	assert(s);
	rtrim(s);
	while (isspace(*s)) {
		s++;
	}
	return s;
}
```

## Trim all white-space from string and truncate spaces

**Example Function:**

[string-helpers.c](src/string-helpers.c)

```
void remove_spaces(char *s)
{
	assert(s);
	char *d = s;
	do {
		while (*d == ' ')
			++d;
	} while ((*s++ = *d++));
}
```

**Example Usage:**

[remove-spaces.c](src/remove-spaces.c)

```
remove-spaces "  foo bar    baz  "
foobarbaz
```

# ARRAYS

## Introduction

There are two aggregate data types in C, whereby multiple elements can be
combined to form a whole - arrays and structures. Arrays hold multiple items of
the exact same data type, whereas structures group variables of different (or
same) data types together under one name.

In this section we are going to look at arrays.

## Fixed-size arrays

In C, a _fixed-size_ array refers to memory that is allocated on the stack at
compile time and does not change size. An example of a use-case in which this is
adequate is in a Sudoku solver where the data could be stored in a 2D array like
`int grid[9][9]`.

### Array-size

When iterating over a fixed-size array, it is important to know its size. For
this purpose an `ARRAY_SIZE` macros is commonly used:

[macros.h](src/macros.h)

```
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
```

### Fixed-size array of strings

[array-fixed-size-strings.c](src/array-fixed-size-strings.c)

```
const char * const words[] = { "Foo", "Bar", "Baz" };

for (size_t i = 0; i < ARRAY_SIZE(words); i++) {
	printf("%s\n", words[i]);
}
```

### Fixed-size array of structs

[array-fixed-size-structs.c](src/array-fixed-size-structs.c)

```
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
```

## Dynamic arrays

### Dynamic array of structs

In this context _dynamic_ means that memory is allocated on the heap at runtime,
and that the size of the array can be changed.

Let us start with an array of structs, using pure C with no macros, helper
libraries, or abstractions.

Assuming that we have `struct item`, we need three variables: The array itself
(`*items`), the length of the array (`nr_items`) and the size of the heap
allocation (`alloc_items`).

> These could of course be wrapped up in a struct and passed between the
functions, but for simplicity we will just declare them as file-scope variables.
In C this means that they are global variables restricted to the scope of a
single translation unit. They are sometimes also known as internal global
variables and are created by applying the _static_ keyword to the variable
declaration outside of any function. As a side note, this is one of the few
language supported ways in which a level of privacy can be achieved in C.

This approach to arrays works well if you do not want to depend on libraries
and can keep the usage within one translation unit.

[array-dynamic-structs.c](src/array-dynamic-structs.c)

```
static struct item *items;
static size_t nr_items, alloc_items;
```

Appending an item to the array then becomes:

```
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
```

```
static void items_append(const char *name)
{
	assert(name);
	struct item *item = add_item();
	item->name = xstrdup(name);
}
```

...which can be used like this:

```
items_append("Foo");
items_append("Bar");
items_append("Baz");
qsort(items, nr_items, sizeof(struct item), compare_item_name);
```

...where the `qsort()` callback looks like:

```
static int compare_item_name(const void *a, const void *b)
{
	const struct item *item_a = a;
	const struct item *item_b = b;
	return strcmp0(item_a->name, item_b->name);
}
```

You can iterate over the array as follows:

```
for (size_t i = 0; i < nr_items; i++) {
	struct item *item = &items[i];
	printf("%s\n", item->name);
}
```

...to generate the output:

```
Bar
Baz
Foo
```

### Dynamic array of strings using glib

Here is a simpler example without the sort function and just adding strings
rather than strings within structs:

[array-dynamic-strings-glib.c](src/array-dynamic-strings-glib.c)

```
g_autoptr(GPtrArray) items = g_ptr_array_new_with_free_func(g_free);

g_ptr_array_add(items, g_strdup("Foo"));
g_ptr_array_add(items, g_strdup("Bar"));
g_ptr_array_add(items, g_strdup("Baz"));

for (guint i = 0; i < items->len; ++i) {
	g_print("%s\n", (gchar *)g_ptr_array_index(items, i));
}
```

...to generate the output:

```
Foo
Bar
Baz
```

In the above example `g_strdup()` was used to provide a more realistic real-life
example of a dynamic array, because if you knew all the strings at build time
you would probably just a fixed-size array.

Automatic cleanup is used through `g_autoptr` which handles freeing the
`GPtrArray` container when exiting the scope. `g_free` is passed to free
dynamically allocated strings inside.

