# Engineering with C

<i>A collection of code snippets to demonstrate ways of working in C</i>

# Table of Contents

## Code Snippets

* [FOREWORD](#foreword)
* [STRINGS](#strings)
    * [Trim leading and trailing white-space from string](#trim-leading-and-trailing-white-space-from-string)
    * [Truncate spaces in string](#truncate-spaces-in-string)
    * [Split a string on a delimiter](#split-a-string-on-a-delimiter)
* [ARRAYS](#arrays)
    * [Fixed-size arrays](#fixed-size-arrays)
    * [Dynamic arrays](#dynamic-arrays)
* [LISTS](#lists)
    * [Singly linked list](#singly-linked-list)
    * [Doubly linked list](#doubly-linked-list)
    * [Sort a list](#sort-a-list)
    * [Reverse a list](#reverse-a-list)
* [MEMORY](#memory)
    * [Storage Duration](#storage-duration)
    * [Stack and Heap](#stack-and-heap)
    * [Lifetime](#lifetime)
    * [Memory Errors](#memory-errors)
    * [Sanitizers](#sanitizers)
    * [Cleanup](#cleanup)

# FOREWORD

The primary goal of this book is to document ways of doing things in C. As a
secondary benefit it also offers reflections on what programming in C teaches us
about Engineering.

It has been inspired by [pure-bash-bible] and [pure-sh-bible] in terms of the
type of code-snippets it includes.

Examples range from very simple, idiomatic approaches without helper libraries
or abstractions, to more sophisticated methods using glib, talloc, cleanup
attributes and the like.

A small number of helpers are used to keep the examples brief and to the point;
for instance `xrealloc()` and `die()`.

Many example functions use `assert()` to document their expectations about
input.  These assertions are only intended to fail if the programmer has made a
mistake, rather than as a means of handling invalid user input. Depending on how
you intend to use the code, it may therefore be better to replace the `assert()`
with more graceful error handling and an appropriate error message.

In general, the code has only been tested on Linux, so feedback is appreciated
if you find any issues on other Operating Systems.

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
	while (end >= s && isspace((unsigned char)*end)) {
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
	while (isspace((unsigned char)*s)) {
		s++;
	}
	return s;
}
```

## Truncate spaces in string

**Example Function:**

[string-helpers.c](src/string-helpers.c)

```
void truncate_spaces(char *s)
{
	assert(s);
	char *d = s;
	do {
		while (*d == ' ' && *(d + 1) == ' ')
			++d;
	} while ((*s++ = *d++));
}
```

**Example Usage:**

[truncate-spaces.c](src/truncate-spaces.c)

```
truncate-spaces "foo       bar    baz"
foo bar baz
```

## Split a string on a delimiter

There are several ways to split a string in C, and the right choice depends on
what you need from the result.

### With `strtok_r()`

`strtok_r()` is a POSIX function and is a fast way of splitting a string without
making any allocations. This can make it a good choice when performance matters,
provided its behaviour matches your requirements.

There are three important things to understand about `strtok_r()`:

1. It modifies the original string, replacing delimiters with `'\0'`.
2. It treats the delimiter argument as a set of individual characters, rather
   than as a complete delimiter string. For example, with the input
   `"apple-orange,banana"` and delimiters `",-"`, it splits on either `-` or
   `,`.
3. It ignores empty fields. For example, `"apple,,banana"` produces only
   `"apple"` and `"banana"`.

The `"_r"` suffix means that the function is reentrant: the state required to
continue tokenising is supplied by the caller through saveptr. This makes it
preferable to `strtok()` when writing code that may be called concurrently or
when multiple strings need to be tokenised at the same time.

**Example Function:**

[split-with-strtok.c](src/split-with-strtok.c)

```
char *saveptr, *token, *str = argv[1], *delim = argv[2];
while ((token = strtok_r(str, delim, &saveptr))) {
	printf("%s\n", token);
	str = NULL;
}
```

**Example Usage:**

```
./src/split-with-strtok "apples,oranges;pears,grapes" ",;"
apples
oranges
pears
grapes
```

### By allocating parts

Sometimes we need something different. We may want to preserve empty fields, or
we may want the resulting strings to have a lifetime independent of the original
string.

In this example, the delimiter is treated as a single, exact sequence of
characters. Splitting therefore occurs only when the complete delimiter
substring is found.

Unlike `strtok_r()`, this implementation preserves empty fields.

**Example Function:**

[string-split.c](src/string-split.c)

```
char **string_split(char *str, const char *delim)
{
	if (!str || !delim || !*delim) {
		return NULL;
	}

	size_t argc = 1;
	char *p = str;

	while ((p = strstr(p, delim))) {
		argc++;
		p += strlen(delim);
	}

	char **argv = xcalloc(argc + 1, sizeof(*argv));

	char **argvp = argv;
	char *start = str;
	size_t delim_len = strlen(delim);

	while ((p = strstr(start, delim))) {
		*argvp++ = strndup(start, p - start);
		start = p + delim_len;
	}

	*argvp++ = xstrdup(start);
	*argvp = NULL;

	return argv;
}
```

**Example Usage:**

[split.c](src/split.c)

```
./src/split "apples,oranges,pears,grapes" ","
apples
oranges
pears
grapes
```

```
./src/split "1, 2, 3, 4, 5" ", "
1
2
3
4
5
```

The delimiter can also be a longer substring:

```
./src/split "hello---world---my---name---is---john" "---"
hello
world
my
name
is
john
```

### Using GLib

If you are already linking with GLib, `g_strsplit()` and `g_strsplit_set()`
provide convenient alternatives. `g_strsplit()` treats its delimiter as a
complete string, while `g_strsplit_set()` treats it as a set of individual
delimiter characters, much like `strtok_r()`.

**Example Usage:**

[split-with-glib.c](src/split-with-glib.c)

```
char **tokens = g_strsplit_set("foo bar\tbaz", " \t", -1);
for (size_t i = 0; tokens[i]; ++i) {
	printf("%s\n", tokens[i]);
}
g_strfreev(tokens);
```

```
foo
bar
baz
```


# ARRAYS

## Introduction

C provides two fundamental aggregate types: arrays, which contain multiple
elements of the same type, and structures, which group multiple members of
different (or the same) type under a single name.

## Fixed-size arrays

In C, a _fixed-size_ array has a number of elements that is fixed when the array
is defined. Its storage may have automatic, static or allocated duration.

### Array-size

When iterating over a fixed-size array, it is important to know its size. For
this purpose, an `ARRAY_SIZE` macro is commonly used:

```
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0])
```

`ARRAY_SIZE` should only be used on arrays, not pointers. The version above
silently continues with incorrect results if a pointer is passed to it, so a
better version - which catches the problem at compile time - is therefore:

[macros.h](src/macros.h)

```
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0])) + __must_be_array(arr)
```

```
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __is_array(a) (!__same_type((a), &(a)[0]))
#define __must_be_array(a) ({ _Static_assert(__is_array(a), "not array"); 0; })
```

This relies on GNU C extensions `__builtin_types_compatible_p()` and `({ ... })`
statement-expression, so if it important not to use gcc or clang then stick with
the version without `__must_be_array()`.

It is very likely that the next C standard (C2y) will contain `_Countof()` which
fundamentally does the same as the above `ARRAY_SIZE()`.  GCC 16 and Clang 21
already have implementation support.

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

Here, _dynamic_ means that the array's storage is allocated at runtime and can
be resized as the number of elements changes.

Let's start with an array of structures, using pure C with no macros, helper
libraries, or abstractions.

Assuming that we have `struct item`, we need three variables: The array itself
(`*items`), the number of elements (`nr_items`) and the allocated capacity
(`alloc_items`). These could of course be wrapped in a struct and passed between
functions, but for simplicity we just declare them as file-scope variables.

> Note: A file-scope variable declared static has internal linkage and is
> visible only within that translation unit.

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
		alloc_items = (alloc_items + 16) * 3 / 2;
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

This approach to arrays works well if you do not want to depend on libraries
and can keep the usage within one translation unit.

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
you would probably just use a fixed-size array.

Automatic cleanup is used through `g_autoptr` which handles freeing the
`GPtrArray` container when exiting the scope. `g_free` is passed to free
dynamically allocated strings inside.

# LISTS

## Singly linked list

Singly linked lists are conceptually simple and are commonly taught as an
introductory data structure.

[list-singly-linked.c](src/list-singly-linked.c)

```
struct item {
	int value;
	struct item *next;
};
```

Elements can then be added with something like:

```
static void list_add(struct item **head, int value)
{
	struct item *item = xcalloc(1, sizeof(*item));
	item->value = value;
	item->next = *head;
	*head = item;
}
```

```
struct item *items = NULL;

const int ints[] = { 4, 3, 2, 1, 0 };
for (size_t i = 0; i < ARRAY_SIZE(ints); ++i) {
	list_add(&items, ints[i]);
}
```

...and finally we can iterate over the list with a simple for-loop:

```
for (struct item *p = items; p; p = p->next) {
	printf("%d\n", p->value);
}
```

We will not cover this type of list further here because it does not enable
generic code, so each implementation has to re-invent the wheel. There are some
well-known projects using them if you want to study this approach:

- git ([commit.h])
- gcc ([obstack.h])

[commit.h]: https://github.com/git/git/blob/593c42fe075be0c8cd5239b3a2f21c610cbc9798/commit.h#L17-L20
[obstack.h]: https://github.com/gcc-mirror/gcc/blob/c3743bccd47712301ade2bc8d85fd3477bfce9ca/include/obstack.h#L156-L161
## Doubly linked list

By embedding a `struct list_head` (or similar) in another structure, the same
list implementation can be used to link objects of any type.

```
struct list_head {
	struct list_head *next, *prev;
};
```

This type of implementation avoids ad hoc linked lists and duplicated
definitions every time a list is needed. The circular doubly-linked setup also
simplifies insertion and removal operations.

Many battle-tested projects and libraries use this type of implementation:

- Linux kernel ([include/linux/list.h])
- Wayland ([wayland-util.h])
- Apache Portable Runtime ([apr_ring.h])
- QEMU ([queue.h])
- systemd ([basic/list.h])
- ccan ([ccan/list/list.h])
- Git ([git/list.h])

[include/linux/list.h]: https://elixir.bootlin.com/linux/v7.2/source/include/linux/list.h
[wayland-util.h]: https://gitlab.freedesktop.org/wayland/wayland/-/blob/main/src/wayland-util.h?ref_type=heads#L247
[apr_ring.h]: https://github.com/apache/apr/blob/trunk/include/apr_ring.h
[queue.h]: https://github.com/qemu/qemu/blob/master/include/qemu/queue.h
[basic/list.h]: https://github.com/systemd/systemd/blob/main/src/basic/list.h
[ccan/list/list.h]: https://github.com/rustyrussell/ccan/blob/master/ccan/list/list.h
[git/list.h]: https://github.com/git/git/blob/master/list.h

The Linux kernel implementation will be used in this book to demonstrate this
approach.

Let's start by looking at a simple example. First, the list needs to be defined
and initialised (`items` in this case).

[list-sort.c](src/list-sort.c)

```
LIST_HEAD(items);
```

Then the structure of interest - the one to be added to the list - needs to be
defined; and it needs to contain a `struct list_head` member.

```
struct item {
	int value;
	struct list_head link;
};
```

Then items can be added:

```
static void add_item(struct list_head *items, int value)
{
	struct item *item = xcalloc(1, sizeof(*item));
	item->value = value;
	list_add_tail(&item->link, items);
}
```

```
const int ints[] = { 4, 2, 9, 1, 7, 0, 5, 3, 8, 6 };
for (size_t i = 0; i < ARRAY_SIZE(ints); ++i) {
	add_item(&items, ints[i]);
}
```

...and iterated over:

```
struct item *item;
list_for_each_entry(item, &items, link) {
	printf("%d", item->value);
}
```

> NOTE 1: The [list_head] structure should not itself hold any other data, but
> should be embedded in the structure to be contained in a list.

> NOTE 2: A [container_of] macro is used to obtain the containing structure from
> a pointer to its `list_head` member. This book does not try to describe how
> this works, but is just mentioning it for completeness.

> NOTE 3: As opposed to the textbook singly list mental model, it is worth
> noting that the actual head of the list tends to be a standalone `list_head`
> structure, rather than one embedded within a structure type of interest.

[container_of]: https://elixir.bootlin.com/linux/v7.2/source/include/linux/container_of.h#L19
[list_head]: https://elixir.bootlin.com/linux/v7.2/source/include/linux/types.h#L204-L206

## Sort a list

The C language has no builtin support for sorting lists, but the Linux kernel
contains a sophisticated [merge-sort](src/list_sort.c) implementation.

Building on the example in the previous section, the list can be sorted with:

[list-sort.c](src/list-sort.c)

```
list_sort(NULL, &items, compare_ints);
```

```
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
```

...to generate the output:

```
0123456789
```

## Reverse a list

The list from the previous example can be reversed in place:

[list-reverse.c](src/list-reverse.c)

```
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
```

```
list_reverse(&items);
```

...to generate the output:

```
9876543210
```

# MEMORY

## Storage Duration

In C, storage duration determines how long an object's storage exists. The
language defines four storage durations: automatic, static, thread, and
allocated. The stack and heap are common implementation mechanisms used to
provide some of these.

## Stack and Heap

Although the C standard describes storage duration, it doesn't prescribe how
that storage must be implemented. In most systems, automatic storage is provided
by the stack, while dynamically allocated storage comes from the heap.

The stack is managed automatically. An automatic object's lifetime ends when
execution leaves the block in which it is declared.  The stack is usually
limited in size, so it is unsuitable for arbitrarily large or long-lived
objects.

The heap provides storage that can be allocated dynamically with functions such
as `malloc()` and `calloc()`. Its lifetime is controlled explicitly by the
program. This flexibility comes at the cost of allocation overhead and the
responsibility for managing the lifetime correctly.

## Lifetime

Lifetime describes the period during which the object exists. For example:

```
void foo(void)
{
	int value = 42;
	/* value exists and can be used here */
}
```

```
void bar(void)
{
	int *value = malloc(sizeof *value);
	*value = 42;
	/*
	 * The pointer and the allocated object have different lifetimes. The
	 * local variable `value` has automatic storage duration. The object it
	 * points to has allocated storage duration.
	 */
	free(value);
}
```

## Memory Errors

Memory errors occur when a program accesses storage incorrectly. Two fundamental
questions are:

1. Is the object still within its lifetime?
2. Is the access within the object's bounds?

For example, accessing an object after `free()` is a lifetime error:

```
int *value = malloc(sizeof *value);
free(value);
printf("%d\n", *value); /* use-after-free */
```

Accessing beyond the end of an object is a bounds error:

```
int values[4];
values[4] = 42; /* out-of-bounds */
```

Another common error is reading a value before it has been initialised:

```
int value;
printf("%d\n", value); /* uninitialised value */
```

These errors can be difficult to diagnose because the invalid access does not
necessarily cause an immediate failure.

## Sanitizers

GCC and Clang provide a family of runtime error detectors called sanitizers.
They instrument a program at compile time and report errors when the
instrumented program runs.

`AddressSanitizer` (ASan) detects memory errors such as buffer overflows,
use-after-free, use-after-scope and double-free.

`UndefinedBehaviorSanitizer` (UBSan) detects various forms of undefined behaviour,
such as signed integer overflow, invalid shifts and misaligned memory accesses.

`LeakSanitizer` (LSan) detects memory leaks, where allocated memory is no longer
reachable. On platforms where LeakSanitizer is integrated with AddressSanitizer,
this enables ASan, UBSan and LSan: 

```
CFLAGS += -g -fsanitize=address,undefined
```

The `-g` option includes debugging information, allowing the sanitizer to
produce diagnostics containing useful source locations such as filenames and
line numbers.

They add runtime and memory overhead, so they are normally only used during
development and testing rather than in production builds.

## Cleanup

Standard C does not currently provide an automatic cleanup mechanism for
allocated objects. Various proposals have explored adding deferred cleanup to C,
but portable C must currently use explicit cleanup, typically with goto, or
compiler-specific extensions.

GCC and Clang support the compiler extension `__attribute__((__cleanup__()))`
which is used by significant projects like `Linux` and `systemd`. The compiler
automatically calls a specified function when then variable's scope ends.

### Example Code

In a lot of existing C code, `free()` is called like in the example below for
any heap allocated memory. 

When a function has several return paths, `goto` is commonly used to funnel them
through a single cleanup path. This is one of the few uses of goto that is
widely considered good practice in C.

[cleanup-traditional.c](src/cleanup-traditional.c)

```
int main(void) {
	char *buffer = xmalloc(4096);
	/* Do something with buffer */
	free(buffer);
}
```

GCC and Clang's cleanup attribute provides a similar scope-based cleanup
mechanism:

[cleanup-simple.c](src/cleanup-simple.c)

```
void freep(char **ptr) {
	free(*ptr);
}
```
```
int main(void) {
	__attribute__((__cleanup__(freep))) char *buffer = xcalloc(1, 4096);
	/* Do something with buffer */
}
```

> Note: The cleanup function receives the address of buffer, so `freep()` takes
> a `char **` and frees the pointer stored there.

Whilst functional, this is quite ugly and verbose. Taking inspiration from
`systemd`, it can be made more succinct and to the point:

[cleanup.h](src/cleanup.h)<br />
[cleanup-better.c](src/cleanup-better.c)

```
int main(void) {
	cleanup_free char *buffer = xcalloc(1, 4096);
	/* Do something with buffer */
}
```

### References

- `Linux` defines a `__cleanup()` macro in [include/linux/compiler_attributes.h]
  which just abbreviates the attribute-cleanup syntax. It then uses that in a
  bunch of helpers in [include/linux/cleanup.h] including `scoped_guard()` and
  `DEFINE_FREE()`.
- `systemd` takes a similar approach defining
    - `_cleanup_` in [systemd/src/fundamental/macro.h]
    - `_cleanup_free_` in [systemd/src/basic/alloc-util.h]
    - `_cleanup_fclose_` in [systemd/src/basic/fd-util.h]

[include/linux/compiler_attributes.h]: https://elixir.bootlin.com/linux/v7.2/source/include/linux/compiler_attributes.h#L76
[include/linux/cleanup.h]: https://elixir.bootlin.com/linux/v7.2/source/include/linux/cleanup.h#L450

[systemd/src/fundamental/macro.h]: https://github.com/systemd/systemd/blob/d1697c4ff8ce4581ccb9db2169477ec59bfc6689/src/fundamental/macro.h#L79
[systemd/src/basic/alloc-util.h]: https://github.com/systemd/systemd/blob/d1697c4ff8ce4581ccb9db2169477ec59bfc6689/src/basic/alloc-util.h#L82
[systemd/src/basic/fd-util.h]: https://github.com/systemd/systemd/blob/d1697c4ff8ce4581ccb9db2169477ec59bfc6689/src/basic/fd-util.h#L113
