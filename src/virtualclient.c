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
#include <unistd.h>
#include "common/signal.h"
#include "common/subscription.h"
#include "common/ringbuffer.h"
#include "client/client.h"
#include "client/signalhelper.h"

#define CMD_UPDATE  0
#define CMD_READ    1
#define CMD_WRITE   2

struct state {
  int  cmd;
  char signal[32];
  int  period;
  int  value;
};

void event_update_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  printf("Updating Name[%s] Val[%d]\n", signal->s_name, signal->s_value);
}

void event_write_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
}

void client_init(struct execution_context_s *ctx, int argc, char **argv) {
  struct state *s = malloc(sizeof(struct state));
  static struct option long_options[] =
  {
    /* These options don’t set a flag.
       We distinguish them by their indices. */
    {"period",  required_argument, 0, 'p'},
    {0, 0, 0, 0}
  };
  int option_index;

  printf("Initializing virtual client\n");
  ctx->clientstate = s;
  s->period = 0;
  s->cmd = 0;
  printf("Client initialized\n");

  while(1) {
    int c = getopt_long(argc, &argv[1], "p:", long_options, &option_index);

    if(c == -1)
      break;

    switch(c) {
    case 0:
      s->cmd = CMD_UPDATE;
      break;
    case 'p':
      s->period = atoi(optarg);
      break;
    }
  }
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
