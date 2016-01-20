/*
 * usage: boomplay [--volume=vol] <fname>
 *
 * if no arg given, control interface on stdin:
 *  - play
 *  - pause (toggles)
 *  - forcepause
 *  - forceresume
 *  - pl_wipe
 *  - pl_add <fname>
 *  - volume <abs|+rel|-rel>
 *
 * TODO:
 *  - gapless
 *  - rva (replaygain?)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <SDL.h>
#include <mpg123.h>

#include "player.h"
#include "util.h"

void control(void)
{
}

int play_now(const char *fname)
{
  player_t *player;

  player = player_new();
  player_playfile(player, fname);
  player_wait(player);
  player_free(player);

  return 0;
}

int main(int argc, char *argv[])
{
  if (mpg123_init() != MPG123_OK) {
    die("mpg123 init");
  }

  if (SDL_Init(SDL_INIT_AUDIO)) {
    die("SDL init");
  }

  if (argc == 2) {
    play_now(argv[1]);
    return EXIT_SUCCESS;
  }

  control();
  return EXIT_SUCCESS;
}
