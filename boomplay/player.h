#ifndef __PLAYER_H
#define __PLAYER_H

typedef struct player player_t;

player_t *player_new(void);
void player_free(player_t *player);

int player_loadfile(player_t *player, const char *fname);
int player_playfile(player_t *player, const char *fname);
int player_play(player_t *player);
int player_wait(player_t *player);
int player_pause(player_t *player);
int player_forcepause(player_t *player);
int player_forceresume(player_t *player);
int player_stop(player_t *player);
int player_volume_set(player_t *player, int volume);
int player_volume_get(player_t *player);

#endif /* __PLAYER_H */
