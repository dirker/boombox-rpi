#include <SDL.h>
#include <mpg123.h>

#include "player.h"

#include "completion.h"
#include "util.h"

struct player {
  mpg123_handle *mh;
  SDL_AudioDeviceID dev;
  completion_t *done;
};

static void audio_callback(void *priv, Uint8* stream, int len)
{
  player_t *player = (player_t *)priv;
  size_t decoded;

  int err = mpg123_read(player->mh, stream, len, &decoded);

  if (decoded < len) {
    Uint8 *p = &stream[decoded];
    SDL_memset(p, 0, len - decoded);
  }

  if (err != MPG123_OK) {
    complete(player->done);
  }
}

player_t *player_new(void)
{
  player_t *player = xcalloc(1, sizeof(*player));

  player->mh = mpg123_new(NULL, NULL);
  if (!player->mh) {
    die("mpg123 new");
  }

  player->done = completion_new();

  return player;
}

void player_free(player_t *player)
{
  /* FIXME:
   *  - stop if playing
   *  - ensure done
   */

  mpg123_delete(player->mh);
  completion_free(player->done);
  free(player);
}

int player_loadfile(player_t *player, const char *fname)
{
  int err;

  err = mpg123_open(player->mh, fname);
  if (err != MPG123_OK) {
    die("mpg123 open");
  }

  return 0;
}

int player_playfile(player_t *player, const char *fname)
{
  player_loadfile(player, fname);
  player_play(player);
  return 0;
}

int player_play(player_t *player)
{
  int err;
  int channels;
  int encoding;
  long rate;

  err = mpg123_getformat(player->mh, &rate, &channels, &encoding);
  if (err != MPG123_OK) {
    die("mpg123 getformat");
  }

  mpg123_format_none(player->mh);
  mpg123_format(player->mh, rate, channels, encoding);

  SDL_AudioSpec want, have;

  SDL_zero(want);
  want.freq = rate;
  want.format = AUDIO_S16;
  want.channels = channels;
  want.samples = 4096;
  want.callback = audio_callback;
  want.userdata = player;

  /* SDL_AUDIO_ALLOW_FORMAT_CHANGE */
  player->dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
  if (!player->dev)
    die("Failed to open audio: %s\n", SDL_GetError());

  SDL_PauseAudioDevice(player->dev, 0); // start audio playing.
  return 0;
}

int player_wait(player_t *player)
{
  wait_for_completion(player->done);
  SDL_CloseAudioDevice(player->dev);
  return 0;
}

int player_pause(player_t *player)
{
  /* TODO */
  return 0;
}

int player_forcepause(player_t *player)
{
  /* TODO */
  return 0;
}

int player_forceresume(player_t *player)
{
  /* TODO */
  return 0;
}
