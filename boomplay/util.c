#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "util.h"

void die(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  exit(EXIT_FAILURE);
}

ssize_t readline(char **linep, size_t *linecapp, FILE *stream)
{
  ssize_t ret;

  ret = getline(linep, linecapp, stream);
  if (ret < 0)
    return ret;

  /* remove trailing newline */
  (*linep)[--ret] = '\0';

  return ret;
}

void *xcalloc(size_t count, size_t size)
{
  void *ret = calloc(count, size);
  if (!ret)
    die("calloc");
  return ret;
}

void *xmalloc(size_t size)
{
  void *ret = malloc(size);
  if (!ret)
    die("malloc");
  return ret;
}
