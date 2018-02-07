/***************************************************
 * virtualclient.c
 * Created on Sun, 22 Oct 2017 19:31:13 +0000 by vladimir
 *
 * $Author$
 * $Rev$
 * $Date$
 ***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/signal.h"
#include "common/subscription.h"
#include "common/ringbuffer.h"
#include "client/client.h"
#include "client/signalhelper.h"
#include "config.h"

void event_update_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  // Should never get here
  printf("*** EVENT HANDLER: Signal %s updating from value %d to value %d\n", signal->s_name, signal->s_value, value);
  signal->s_value = value;
}

void event_write_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  // Check config value
  int result = 0;
  printf("*** EVENT HANDLER: Signal %s is written value %d\n", signal->s_name, value);

  if((result = config_check(ctx->clientstate, signal->s_name, value)) == 0) {
    signal->s_value = value;
    result = config_save(signal->s_name, value, "config.conf");
    post_update_command(ctx, signal->s_name, value);
  }
  
  printf("*** Posting update status %d\n", result);
  post_update_command(ctx, "dev.conf.updateStatus", result);
  post_process(ctx);
}

void client_init(struct execution_context_s *ctx, int argc, char **argv) {
  struct signal_s *s;
  struct conf_ctx *c = malloc(sizeof(struct conf_ctx));
  int i;
  c->limits_size = 0;
  c->limits = NULL;

  hash_create(&c->limits_hash);
  config_load_limits(c, "limits.conf");

  for(i = 0; i < c->limits_size; i ++) {
    printf("Loaded limit %s: %d, %d\n", c->limits[i].name, c->limits[i].low, c->limits[i].high);
    hash_add(c->limits_hash, c->limits[i].name, &c->limits[i]);
  }

  ctx->clientstate = c;
  get_and_subscribe(ctx, "dev.conf", SUB_WRITE);
  subscribe_command(ctx, "dev.conf", SUB_UPDATE);
  config_load(ctx, "config.conf");
}

// Polling modbus devices
void client_thread_proc(struct execution_context_s *ctx) {
  printf("Started config client proc thread\n");
  while(ctx->running) {
    sleep(1);
    post_process(ctx);
  }
}
