# Engineering with C

<i>A collection of code snippets to demonstrate ways of working in C</i>

# Table of Contents

* [FOREWORD](#foreword)
* [STRINGS](#strings)
    * [Trim leading and trailing white-space from string](#trim-leading-and-trailing-white-space-from-string)
    * [Truncate spaces in string](#truncate-spaces-in-string)
* [ARRAYS](#arrays)
    * [Fixed-size arrays](#fixed-size-arrays)
    * [Dynamic arrays](#dynamic-arrays)
* [LISTS](#lists)
    * [Singly linked list](#singly-linked-list)
    * [Doubly linked list](#doubly-linked-list)
    * [Sort a list](#sort-a-list)
    * [Reverse a list](#reverse-a-list)

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

In general, the code is intended to be portable but it has only been tested on
Linux, so feedback is appreciated if you find any issues on other Operating
Systems.

If you see something that is wrong, incorrectly described or missing I would be
grateful for a github issue or pull request.

[pure-bash-bible]: https://github.com/dylanaraps/pure-bash-bible
[pure-sh-bible]: https://github.com/dylanaraps/pure-sh-bible

# STRINGS

## Trim leading and trailing white-space from string

**Example Function:**

[string-helpers.c](src/string-helpers.c)

@code:src/string-helpers.c:0

@code:src/string-helpers.c:1

## Truncate spaces in string

**Example Function:**

[string-helpers.c](src/string-helpers.c)

@code:src/string-helpers.c:2

**Example Usage:**

[truncate-spaces.c](src/truncate-spaces.c)

```
truncate-spaces "foo       bar    baz"
@exec:./src/truncate-spaces "foo       bar    baz"
```

# ARRAYS

## Introduction

C provides two fundamental aggregate types: arrays, which contain multiple
elements of the same type, and structures, which group multiple members under a
single name.

## Fixed-size arrays

In C, a _fixed-size_ array has a number of elements that is fixed when the array
is defined. Its storage may have automatic, static or allocated duration. An
example of a use-case in which this is adequate is in a Sudoku solver where the
data could be stored in a 2D array like `int grid[9][9]`.

### Array-size

When iterating over a fixed-size array, it is important to know its size. For
this purpose, an `ARRAY_SIZE` macro is commonly used:

[macros.h](src/macros.h)

@code:src/macros.h:0

> Note: This works only when `arr` is an actual array, not when it is a pointer.

### Fixed-size array of strings

[array-fixed-size-strings.c](src/array-fixed-size-strings.c)

@code:src/array-fixed-size-strings.c:0

### Fixed-size array of structs

[array-fixed-size-structs.c](src/array-fixed-size-structs.c)

@code:src/array-fixed-size-structs.c:0

## Dynamic arrays

### Dynamic array of structs

Here, _dynamic_ means that the array's storage is allocated at runtime and can
be resized as the number of elements changes.

Let's start with an array of structures, using pure C with no macros, helper
libraries, or abstractions.

Assuming that we have `struct item`, we need three variables: The array itself
(`*items`), the number of elements (`nr_items`) and the allocated capacity
allocation (`alloc_items`). These could of course be wrapped in a struct and
passed between functions, but for simplicity we just declare them as file-scope
variables.

> Note: A file-scope variable declared static has internal linkage and is
> visible only within that translation unit.

[array-dynamic-structs.c](src/array-dynamic-structs.c)

@code:src/array-dynamic-structs.c:0

Appending an item to the array then becomes:

@code:src/array-dynamic-structs.c:1

@code:src/array-dynamic-structs.c:2

...which can be used like this:

@code:src/array-dynamic-structs.c:3

...where the `qsort()` callback looks like:

@code:src/array-dynamic-structs.c:4

You can iterate over the array as follows:

@code:src/array-dynamic-structs.c:5

...to generate the output:

```
@exec:./src/array-dynamic-structs
```

This approach to arrays works well if you do not want to depend on libraries
and can keep the usage within one translation unit.

### Dynamic array of strings using glib

Here is a simpler example without the sort function and just adding strings
rather than strings within structs:

[array-dynamic-strings-glib.c](src/array-dynamic-strings-glib.c)

@code:src/array-dynamic-strings-glib.c:0

...to generate the output:

```
@exec:./src/array-dynamic-strings-glib
```

In the above example `g_strdup()` was used to provide a more realistic real-life
example of a dynamic array, because if you knew all the strings at build time
you would probably just a fixed-size array.

Automatic cleanup is used through `g_autoptr` which handles freeing the
`GPtrArray` container when exiting the scope. `g_free` is passed to free
dynamically allocated strings inside.

# LISTS

## Singly linked list

Singly linked lists are conceptually simple and are commonly taught as an
introductory data structure.

[list-singly-linked.c](src/list-singly-linked.c)

@code:src/list-singly-linked.c:0

Elements can then be added with something like:

@code:src/list-singly-linked.c:1

@code:src/list-singly-linked.c:2

...and finally we can iterate over the list with a simple for-loop:

@code:src/list-singly-linked.c:3

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

@code:src/list-sort.c:0

Then the structure of interest - the one to be added to the list - needs to be
defined; and it needs to contain a `struct list_head` member.

@code:src/list-sort.c:1

Then items can be added:

@code:src/list-sort.c:2

@code:src/list-sort.c:3

...and iterated over:

@code:src/list-sort.c:4

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

@code:src/list-sort.c:5

@code:src/list-sort.c:6

...to generate the output:

```
@exec:./src/list-sort
```

## Reverse a list

The list from the previous example can be reversed in place:

[list-reverse.c](src/list-reverse.c)

@code:src/list-reverse.c:0

@code:src/list-reverse.c:1

...to generate the output:

```
@exec:./src/list-reverse
```

