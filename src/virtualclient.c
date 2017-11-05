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
	printf("Updating Name[%s] Val[%d]\n", signal->s_name, signal->s_value);
}

void event_write_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
}

void client_init(struct execution_context_s *ctx, int argc, char **argv) {
	printf("Initializing virtual client\n");
	get_and_subscribe(ctx, "dev.panel10.system_water_flow", SUB_UPDATE);
	ctx->clientstate = NULL;
	printf("Client initialized\n");
}

void client_thread_proc(struct execution_context_s *ctx) {
  printf("Started proc thread\n");
	int val = 0;
  while(ctx->running) {
		post_update_command(ctx, "dev.panel10.system_water_flow", ++val);
		printf("Updating signal to value %d\n", val);
    post_process(ctx);
    sleep(1);
  }
}
