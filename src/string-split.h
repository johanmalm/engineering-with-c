/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef STRING_SPLIT_H
#define STRING_SPLIT_H

void argv_free(char **parts);
char **string_split(char *str, const char *delim);
char *string_join(char **parts, const char *delim);
char *string_replace(char *str, const char *old, const char *new);

#endif /* STRING_SPLIT_H */
