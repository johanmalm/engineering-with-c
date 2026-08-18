#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>

void info(const char *err, ...);
void warn(const char *err, ...);
void die(const char *err, ...);

char *xstrdup(const char *s);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xcalloc(size_t nb, size_t size);

#endif /* UTIL_H */
