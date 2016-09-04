/*
 * usage: boomplay [--volume=vol] <fname>
 *
 * if no arg given, control interface on stdin:
 *  - play
 *  - pause (toggles)
 *  - forcepause
 *  - forceresume
 *  - volume <abs|+rel|-rel>
 *
 * later:
 *  - pl_wipe
 *  - pl_add <fname>
 *
 * TODO:
 *  - unicode
 *  - gapless
 *  - rva (replaygain?)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <poll.h>

#include <SDL.h>
#include <mpg123.h>

#include "player.h"
#include "util.h"

static SDL_mutex *lock;
static player_t *player;
static int done_pipe[2];

static int wait_thread(void *data)
{
  player_wait(player);
  close(done_pipe[1]);
  return 0;
}

static int cmd_pause(const char *cmd)
{
  SDL_LockMutex(lock);

  if (player)
    player_pause(player);

  SDL_UnlockMutex(lock);
  return 0;
}

static int cmd_forcepause(const char *cmd)
{
  SDL_LockMutex(lock);

  if (player)
    player_forcepause(player);

  SDL_UnlockMutex(lock);
  return 0;
}

static int cmd_forceresume(const char *cmd)
{
  SDL_LockMutex(lock);

  if (player)
    player_forceresume(player);

  SDL_UnlockMutex(lock);
  return 0;
}

#if 0
static void cmd_playlist_wipe(const char *cmd)
{
  SDL_LockMutex(lock);

  playlist_wipe();

  if (playthread) {
    player_stop(player);
    SDL_WaitThread(playthread, NULL);
  }

  SDL_UnlockMutex(lock);
}

static void cmd_playlist_add(const char *cmd)
{
  char *fname = strchr(cmd, ' ');

  while (!isalpha(*fname))
    fname++;

  printf("add: '%s'\n", fname);

  SDL_LockMutex(lock);
  playlist_add(fname);
  SDL_UnlockMutex(lock);
}
#endif

static int cmd_volume(const char *cmd)
{
  char *arg = strchr(cmd, ' ');
  unsigned long volume = strtoul(arg, NULL, 10);

  printf("%s(): volume = %lu\n", __func__, volume);

  SDL_LockMutex(lock);
  player_volume_set(player, volume);
  SDL_UnlockMutex(lock);
  return 0;
}

static int cmd_quit(const char *cmd)
{
  return -1;
}

typedef int (*cmd_handler_t)(const char *cmd);

struct cmd {
  const char *name;
  cmd_handler_t handler;
};

struct cmd commands[] = {
  { "pause", cmd_pause },
  { "forcepause", cmd_forcepause },
  { "forceresume", cmd_forceresume },
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

int control(void)
{
  int ret = 0;
  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;

  linelen = readline(&line, &linecap, stdin);
  if (linelen <= 0)
    goto out;

  cmd_handler_t handler = cmd_find_handler(line);
  if (handler)
    ret = handler(line);

out:
  free(line);
  return ret;
}

#if 0
int play_now(const char *fname)
{
  player_t *player;

  player = player_new();
  player_playfile(player, fname);
  player_wait(player);
  player_free(player);

  return 0;
}
#endif

int main(int argc, char *argv[])
{
  char **argp;
  char *fname;
  unsigned int volume = 0;
  int ret;
  struct pollfd pollfd[2];

  if (argc < 2) {
    die("usage: [--volume=<vol>] <fname>\n");
  }

  argp = &argv[1];
  if (argc == 3 && strncmp("--volume=", *argp, 9) == 0) {
    volume = strtoul((*argp)+9, NULL, 0);
    if (volume > 100)
      volume = 100;
    argp++;
  }

  fname = *argp;

  if (mpg123_init() != MPG123_OK) {
    die("mpg123_init\n");
  }

  if (SDL_Init(SDL_INIT_AUDIO)) {
    die("SDL_Init\n");
  }

  ret = pipe(done_pipe);
  if (ret < 0) {
    die("pipe\n");
  }

  pollfd[0].fd = STDIN_FILENO;
  pollfd[0].events = POLLIN;
  pollfd[1].fd = done_pipe[0];
  pollfd[1].events = POLLIN | POLLHUP;

  player = player_new();
  player_loadfile(player, fname);
  player_volume_set(player, volume);
  player_volume_get(player);
  player_play(player);

  SDL_Thread *t = SDL_CreateThread(wait_thread, NULL, NULL);
  if (!t) {
    die("thread creation failed\n");
  }

  for (;;) {
    ret = poll(pollfd, 2, -1);
    if (ret < 0) {
      die("poll\n");
    }

    if (pollfd[0].revents) {
      ret = control();
      player_volume_get(player);
      if (ret)
        break;
    }

    if (pollfd[1].revents)
      break;
  }

  player_stop(player);
  return EXIT_SUCCESS;
}
