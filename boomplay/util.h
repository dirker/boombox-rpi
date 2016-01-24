#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <stdint.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

void die(const char *fmt, ...);
ssize_t readline(char **linep, size_t *linecapp, FILE *stream);
void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);

#endif /* __UTIL_H */
