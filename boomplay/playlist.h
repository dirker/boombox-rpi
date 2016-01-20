#ifndef __PLAYLIST_H
#define __PLAYLIST_H

int playlist_add(const char *fname);
int playlist_seek(unsigned int pos);
const char *playlist_next(void);
void playlist_wipe(void);

#endif
