#include <assert.h>
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
  player_stop(player);
  mpg123_delete(player->mh);
  completion_free(player->done);
  free(player);
}

int player_loadfile(player_t *player, const char *fname)
{
  int err;

  err = mpg123_open(player->mh, fname);
  if (err != MPG123_OK) {
    die("mpg123_open\n");
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
    die("mpg123_getformat\n");
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

  if (player->dev) {
    SDL_CloseAudioDevice(player->dev);
    player->dev = 0;
  }

  return 0;
}

int player_pause(player_t *player)
{
  assert(player->dev);

  SDL_AudioStatus status = SDL_GetAudioDeviceStatus(player->dev);

  switch (status) {
  case SDL_AUDIO_PLAYING:
    SDL_PauseAudioDevice(player->dev, 1);
    break;
  case SDL_AUDIO_PAUSED:
    SDL_PauseAudioDevice(player->dev, 0);
    break;
  default:
    break;
  }

  return 0;
}

int player_forcepause(player_t *player)
{
  assert(player->dev);

  SDL_PauseAudioDevice(player->dev, 1);
  return 0;
}

int player_forceresume(player_t *player)
{
  assert(player->dev);

  SDL_PauseAudioDevice(player->dev, 0);
  return 0;
}

int player_stop(player_t *player)
{
  if (player->dev) {
    SDL_CloseAudioDevice(player->dev);
    player->dev = 0;
  }

  complete(player->done);
  return 0;
}

int player_volume_set(player_t *player, int volume)
{
  assert(player->mh);

  double vol = volume / 100.f;
  mpg123_volume(player->mh, vol);
  return 0;
}

int player_volume_get(player_t *player)
{
  assert(player->mh);

  double base, really, rva_db;
  mpg123_getvolume(player->mh, &base, &really, &rva_db);

  printf("%s(): base = %f, really = %f, rva_db = %f\n", __func__, base, really, rva_db);

  return 0;
}
