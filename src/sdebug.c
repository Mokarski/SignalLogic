/***************************************************
 * virtualclient.c
 * Created on Sun, 22 Oct 2017 19:31:13 +0000 by vladimir
 *
 * $Author$
 * $Rev$
 * $Date$
 ***************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "common/signal.h"
#include "common/subscription.h"
#include "common/ringbuffer.h"
#include "client/client.h"
#include "client/signalhelper.h"

#define CMD_UPDATE  0
#define CMD_READ    1
#define CMD_WRITE   2

int period = 0, subscribe_update = 0, subscribe_write = 0;

void output_signals(struct execution_context_s *ctx, char *name, char *color, char *substitute, int new_value) {
  struct signal_s *s;
  s = ctx->signals;
  printf("\e[H");
  while(s) {
    if(!strcmp(name, s->s_name))
      printf("%s%.8s\e[0m: %45s = %d ==> %d", color, substitute, s->s_name, s->s_value, new_value);
    else
      printf("\e[38;5;0m\e[48;5;11m  Signal\e[0m: %45s = %d", s->s_name, s->s_value);
    s = s->next;
    printf("\e[0K\n");
  }
}

void event_update_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  if(period) {
    output_signals(ctx, signal->s_name, "\e[38;5;15m\e[48;5;2m", "Updating", value);
  }
  else
  {
    printf("\e[38;5;15m\e[48;5;2mUpdating\e[0m: %45s = %d ==> %d\n", signal->s_name, signal->s_value, value);
  }
}

void event_write_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  if(period) {
    output_signals(ctx, signal->s_name, "\e[38;5;15m\e[48;5;4m", "Writing", value);
  }
  else
  {
    printf("\e[38;5;15m\e[48;5;4m Writing\e[0m: %45s = %d ==> %d\n", signal->s_name, signal->s_value, value);
  }
}

void output_help() {
  printf("Usage: sdebug <server-ip> [options]\n");
  printf("Options:\n");
  printf("--period,-p <period>        show top-like with refresh period, ms\n");
  printf("--mask,-m <mask>            signal mask\n");
  printf("--list,-l                   list signals with values\n");
  printf("--updates,-u                subscribe to signals update events\n");
  printf("--writes,-w                 subscribe to signals write events\n");
  printf("--send,-s <value>           write signal value\n");
  printf("--help,-h                   output help\n");
}

void client_init(struct execution_context_s *ctx, int argc, char **argv) {
  static struct option long_options[] =
  {
    /* These options don’t set a flag.
       We distinguish them by their indices. */
    {"period",  1, 0, 'p'},
    {"mask",    1, 0, 'm'},
    {"check",   0, 0, 'c'},
    {"list",    0, 0, 'l'},
    {"updates", 0, 0, 'u'},
    {"writes",  0, 0, 'w'},
    {"send",    1, 0, 's'},
    {"help",    1, 0, 'h'},
    {0, 0, 0, 0}
  };
  int option_index;
  int list = 0, check = 0;
  int write_value = -1;
  char *signalmask[512] = {"dev.", 0};
  int masks = 0, i;

  ctx->clientstate = NULL;

  while(1) {
    int c = getopt_long(argc, argv, "hp:m:cluws:", long_options, &option_index);

    if(c == -1)
      break;

    switch(c) {
    case 'p':
      period = atoi(optarg);
      break;
    case 'u':
      subscribe_update = 1;
      break;
    case 'w':
      subscribe_write = 1;
      break;
    case 'm':
      signalmask[masks ++] = optarg;
      break;
    case 'l':
      list = 1;
      break;
    case 'c':
      check = 1;
      break;
    case 's':
      write_value = atoi(optarg);
      break;
    case 'h':
      ctx->running = 0;
      output_help();
      return;
    }
  }

  if(masks == 0) masks ++;

  if(subscribe_update || period) {
    for(i = 0; i < masks; i ++)
      subscribe(ctx, signalmask[i], SUB_UPDATE);
  }

  if(subscribe_write) {
    for(i = 0; i < masks; i ++)
      subscribe(ctx, signalmask[i], SUB_WRITE);
  }

  if(write_value >= 0) {
    for(i = 0; i < masks; i ++)
      write_command(ctx, signalmask[i], write_value);
  }

  for(i = 0; i < masks; i ++) {
    get_signals(ctx, signalmask[i]);
  }

  if(!subscribe_write && !subscribe_update && !period) {
    ctx->running = 0;
  }

  if(list && !period) {
    struct signal_s *s;
    s = ctx->signals;
    while(s) {
      printf("\e[38;5;0m\e[48;5;11m  Signal\e[0m: %45s = %d\n", s->s_name, s->s_value);
      s = s->next;
    }
  }
  else if(period)
  {
    printf("\e[2J");
    output_signals(ctx, "none", "", "", 0);
  }
}

void client_thread_proc(struct execution_context_s *ctx) {
  int val = 0;
  while(ctx->running) {
    post_process(ctx);
    if(period) {
      usleep(1000 * period);
      output_signals(ctx, "none", "", "", 0);
    }
    else
    {
      break;
    }
  }
}
