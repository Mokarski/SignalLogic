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
#include "logic/keyboard.h"
#include "client/client.h"
#include "logic/logic_client.h"
#include "logic/processor.h"

void event_update_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	static int oil_pump_started = 0;
	if(processor_do(ctx, signal, value)) {
		printf("Processor: signal %s processed\n", signal->s_name);
		return;
	}
	if(
		!strncmp(signal->s_name, "dev.485.kb", 10) ||
		!strncmp(signal->s_name, "dev.485.rpdu", 12) ||
		!strncmp(signal->s_name, "dev.485.pukonv", 12) 
	 ) {
		printf("Updating signal [%s] value [%d => %d]\n", signal->s_name, signal->s_value, value);
		signal->s_value=value; //new value
		process_loop();
	} else {
		static int initialized = 0;

		//while (1){
		if(!initialized) {
			initialized = Init();
			return;
		}
	}
}

void event_write_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
//  printf("*** EVENT HANDLER: Writing value %d to signal %s\n", value, signal->s_name);
//  post_update_command(ctx, "dev.485.rsrs2.sound2_ledms", value);
	//process_loop();
}

void client_init(struct execution_context_s *ctx, int argc, char **argv) {
  printf("Initializing virtual client\n");
	struct logic_context_s *context = malloc(sizeof(struct logic_context_s));
	g_Ctx = ctx;
	get_and_subscribe(ctx, "dev", SUB_UPDATE);
	hash_create(&context->proc_hash);
  ctx->clientstate = context;
	process_loop();
	process_joystick_register(ctx);
  printf("Client initialized\n");
}

void client_thread_proc(struct execution_context_s *ctx) {
  printf("Started proc thread\n");
	initDevices();
  while(ctx->running) {
		abort();
  }
}
