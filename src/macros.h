/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef MACROS_H
#define MACROS_H

//! [0]
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
//! [0]

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#endif /* MACROS_H */
