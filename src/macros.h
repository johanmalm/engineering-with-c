/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef MACROS_H
#define MACROS_H

//! [1]
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __is_array(a) (!__same_type((a), &(a)[0]))
#define __must_be_array(a) ({ _Static_assert(__is_array(a), "not array"); 0; })
//! [1]

//! [0]
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0])) + __must_be_array(arr)
//! [0]

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#endif /* MACROS_H */
