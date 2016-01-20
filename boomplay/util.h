#ifndef __UTIL_H
#define __UTIL_H

void die(const char *fmt, ...);
void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);

#endif /* __UTIL_H */
