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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
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
		//printf("Updating signal [%s] value [%d => %d]\n", signal->s_name, signal->s_value, value);
		//signal->s_value=value; //new value
		//process_loop();
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
	memset(context, 0, sizeof(struct logic_context_s));
	ring_buffer_init(&context->command_buffer);
  socketpair(AF_LOCAL, SOCK_STREAM, 0, context->event_socket);
	g_Ctx = ctx;
	get_and_subscribe(ctx, "dev", SUB_UPDATE);
	hash_create(&context->proc_hash);
  ctx->clientstate = context;
	Init_Worker();
	process_loop();
	process_joystick_register(ctx);
	process_register_common(ctx);
	process_local_post_register(ctx);
  printf("Client initialized\n");
}

void client_thread_proc(struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
  fd_set socks;
  printf("Started proc thread\n");
	initDevices();

  while(ctx->running) {
    FD_ZERO(&socks);
    FD_SET(context->event_socket[0], &socks);

    if(select(context->event_socket[0] + 1, &socks, NULL, NULL, NULL) < 0) {
      continue;
    }

    if(FD_ISSET(context->event_socket[0], &socks)) {
      char localbuf[127];
      read(context->event_socket[0], localbuf, sizeof(localbuf));
    }

    while(ring_buffer_size(context->command_buffer) > 0) {
			struct logic_command_s *command = ring_buffer_get(context->command_buffer);
      ring_buffer_pop(context->command_buffer);

			if(command->command) {
				command->command(command->signal, command->value, ctx);
			}

			free(command);
		}

		//Worker(NULL);
		post_process(ctx);
  }
}
