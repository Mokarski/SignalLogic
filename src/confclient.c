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



void event_update_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  // Should never get here
  printf("*** EVENT HANDLER: Signal %s updating from value %d to value %d\n", signal->s_name, signal->s_value, value);
}

void event_write_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  // Check config value
  printf("*** EVENT HANDLER: Signal %s is written value %d\n", signal->s_name, value);
}

void client_init(struct execution_context_s *ctx, int argc, char **argv) {
  struct signal_s *s;

  get_and_subscribe(ctx, "dev.conf", SUB_WRITE);
}

// Polling modbus devices
void client_thread_proc(struct execution_context_s *ctx) {
  printf("Started config client proc thread\n");
	clock_gettime(CLOCK_REALTIME, &client->last_write);
  while(ctx->running) {
    sleep(1);
    post_process(ctx);
  }
}
