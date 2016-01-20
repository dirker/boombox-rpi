#ifndef __COMPLETION_H
#define __COMPLETION_H

typedef struct completion completion_t;

completion_t *completion_new(void);
void completion_free(completion_t *completion);

void complete(completion_t *completion);
void wait_for_completion(completion_t *completion);

#endif /* __COMPLETION_H */
