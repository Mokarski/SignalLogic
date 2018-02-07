/***************************************************
 * virtualclient.c
 * Created on Sun, 22 Oct 2017 19:31:13 +0000 by vladimir
 *
 * $Author$
 * $Rev$
 * $Date$
 ***************************************************/

#include "client/client.h"
#include "common/signal.h"

#define LOW_LIMIT_PASS    1
#define HIGH_LIMIT_PASS   2
#define SAVE_FAILED       4

struct conf_limits {
  char *name;
  int low;
  int high;
};

struct conf_ctx {
  int limits_size;
  struct conf_limits *limits;
  struct hash_s *limits_hash;
};

int config_load(struct execution_context_s *ctx, char *filename);
int config_load_limits(struct conf_ctx *ctx, char *filename);
int config_check(struct conf_ctx *c, char *name, int value);
int config_save(char *name, int value, char *filename);
