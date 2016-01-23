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
#include <string.h>
#include <ctype.h>

#include <SDL.h>
#include <mpg123.h>

#include "player.h"
#include "util.h"

static void cmd_play(const char *cmd)
{
  /* TODO */
}

static void cmd_pause(const char *cmd)
{
  /* TODO */
}

static void cmd_forcepause(const char *cmd)
{
  /* TODO */
}

static void cmd_forceresume(const char *cmd)
{
  /* TODO */
}

static void cmd_playlist_wipe(const char *cmd)
{
  /* TODO */
}

static void cmd_playlist_add(const char *cmd)
{
  /* TODO */
}

static void cmd_volume(const char *cmd)
{
  /* TODO */
}

static void cmd_quit(const char *cmd)
{
  /* TODO: cleanup state */
  exit(EXIT_SUCCESS);
}

typedef void (*cmd_handler_t)(const char *cmd);

struct cmd {
  const char *name;
  cmd_handler_t handler;
};

struct cmd commands[] = {
  { "play", cmd_play },
  { "pause", cmd_pause },
  { "forcepause", cmd_forcepause },
  { "forceresume", cmd_forceresume },
  { "playlist_wipe", cmd_playlist_wipe },
  { "playlist_add", cmd_playlist_add },
  { "volume", cmd_volume },
  { "quit", cmd_quit },
};

cmd_handler_t cmd_find_handler(const char *str)
{
  char *space = strchr(str, ' ');
  unsigned int cmdlen = 0;
  int i;

  if (space)
    cmdlen = space - str;

  for (i = 0; i < ARRAY_SIZE(commands); i++) {
    struct cmd *cmd = &commands[i];
    int res;

    if (cmdlen)
      res = strncmp(cmd->name, str, cmdlen);
    else
      res = strcmp(cmd->name, str);

    if (res != 0)
      continue;

    return cmd->handler;
  }

  return NULL;
}

void control(void)
{
  char *line;
  size_t linecap = 0;
  ssize_t linelen;

  while ((linelen = readline(&line, &linecap, stdin)) > 0) {
    cmd_handler_t handler = cmd_find_handler(line);

    if (handler)
      handler(line);
  }

out:
  free(line);
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
