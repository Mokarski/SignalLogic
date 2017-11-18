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
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;

	if(processor_do(ctx, signal, value)) {
		printf("Processor: signal %s processed\n", signal->s_name);
		return;
	} else if(context->initialized) {
		signal->s_value = value;
		control_all(signal, value, ctx);
	}

	if(!context->initialized) {
		signal->s_value = value;
		context->initialized = init(signal, value, ctx);
		if(context->initialized) {
			process_joystick_register(ctx);
			process_register_common(ctx);
			process_local_post_register(ctx);
      process_gauge_register(ctx);
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
	process_local_post_switch_register(ctx);
  printf("Client initialized\n");
	context->initialized = init(NULL, 0, ctx);
}

void client_thread_proc(struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
  fd_set socks;

  printf("Stopping all\n");
	stop_all(NULL, 0, ctx);
  printf("Stopping sirens\n");
	control_sirens(ctx, 0);
  printf("Post processing\n");
	post_process(ctx);

  printf("Started proc thread\n");

  while(ctx->running) {
		printf("Running proc thread\n");
    FD_ZERO(&socks);
    FD_SET(context->event_socket[0], &socks);

    if(select(context->event_socket[0] + 1, &socks, NULL, NULL, NULL) < 0) {
      continue;
    }

    if(FD_ISSET(context->event_socket[0], &socks)) {
      char localbuf[127];
      read(context->event_socket[0], localbuf, sizeof(localbuf));
    }

		printf("Processing posted commands in proc thread\n");
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
