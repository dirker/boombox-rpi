#include <stdlib.h>
#include <string.h>

#include "playlist.h"

#define PLAYLIST_MAX_ENTRIES 128

struct playlist {
  const char *entries[PLAYLIST_MAX_ENTRIES];
  unsigned int num_entries;
  unsigned int pos;
};

static struct playlist pl;

int playlist_add(const char *fname)
{
  if (pl.num_entries == PLAYLIST_MAX_ENTRIES)
    return -1;

  pl.entries[pl.num_entries++] = strdup(fname);
  return 0;
}

int playlist_seek(unsigned int pos)
{
  if (pos >= pl.num_entries)
    return -1;

  pl.pos = pos;
  return 0;
}

void playlist_wipe(void)
{
  const char *p;

  playlist_seek(0);
  while ((p = playlist_next())) {
    free((void *)p);
  }

  memset(&pl, 0, sizeof(pl));
}

const char *playlist_next(void) {
  if (pl.pos == pl.num_entries)
    return NULL;

  return pl.entries[pl.pos++];
}
