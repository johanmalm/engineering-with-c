#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

void rtrim(char *s);
char *string_strip(char *s);
void truncate_spaces(char *s);
int strcmp0(const char *a, const char *b);
int strcasecmp0(const char *a, const char *b);

#endif /* STRING_HELPERS_H */
