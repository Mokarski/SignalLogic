#include <stdio.h>
#include <client/signalhelper.h>
#include <common/signal.h>
#include <client/clientcommand.h>
#include <logic/logic_client.h>
#include "processor.h"
#include "config.h"

#define SIGNAL_NUM  512
#define MOTORS 			7

void process_config_register(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.config.wago", &process_config_wago);
	processor_add(ctx, "dev.config.sound", &process_config_sound);

  process_config_sound(NULL, 0, ctx);
}

void process_config_wago(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  int i, snum;
  struct signal_s *signals[SIGNAL_NUM];
  char name[256];

  snum = hash_find_all(ctx->hash, "dev.wago.config", (void**)signals, sizeof(signals) / sizeof(signals[0]));

  for(i = 0; i < snum; i ++) {
    if(signals[i]->s_rw) {
      write_command(ctx, signals[i]->s_name, 0);
    }
  }

  for(i = 1; i < MOTORS + 1; i ++) {
    sprintf(name, "dev.wago.config.m%d.ready", i);
    write_command(ctx, name, 1);
  }
}

void process_config_sound(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  int led_period = signal_get(ctx, "dev.conf.sound.blink_period");
  printf("Setting alarm led period to %d\n", led_period);
  if(led_period) {
    post_write_command(ctx, "dev.485.rsrs2.sound1_ledms", led_period);
    post_write_command(ctx, "dev.485.rsrs2.sound2_ledms", led_period);
  }
  post_process(ctx);
}
