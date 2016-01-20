#include <SDL.h>

#include "completion.h"
#include "util.h"

struct completion {
  unsigned int done;
  SDL_mutex *mutex;
  SDL_cond *cond;
};

completion_t *completion_new(void)
{
  completion_t *c = xcalloc(1, sizeof(*c));

  c->mutex = SDL_CreateMutex();
  c->cond = SDL_CreateCond();

  return c;
}

void completion_free(completion_t *c)
{
  SDL_DestroyCond(c->cond);
  SDL_DestroyMutex(c->mutex);
  free(c);
}

void complete(completion_t *c)
{
  SDL_LockMutex(c->mutex);
  c->done = 1;
  SDL_CondBroadcast(c->cond);
  SDL_UnlockMutex(c->mutex);
}

void wait_for_completion(completion_t *c)
{
  SDL_LockMutex(c->mutex);
  while (!c->done)
    SDL_CondWait(c->cond, c->mutex);
  SDL_UnlockMutex(c->mutex);
}
