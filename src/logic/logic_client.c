/*
 *   C ECHO client example using sockets
 */
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <stdlib.h>

#define  DEBUG 0 //may be 0,1,2,3
#include <errno.h>
#include <unistd.h>
#include "logic_client.h"
#include "process.h"
#include "keyboard.h"
#include "../common/journal.h"

#define Devices 40
#define SignalsPerDev 100

#define CONTROL_MASK		0x3
#define CONTROL_RADIO		0x1
#define	CONTROL_MANU		0x2
#define CONTROL_CABLE		0x3

#define MODE_MASK				(0x03 << 2)
#define MODE_DIAG				(0x01 << 2)
#define MODE_PUMP				(0x02 << 2)
#define MODE_NORM				(0x03 << 2)
#define MODE_STOP				128

int  is_oil_station_started(struct execution_context_s *ctx) {
	if(signal_get(ctx, "dev.wago.oc_mdi1.oc_w_k2")) {
		return 1;
	}

	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	return context->diagnostic;
}

void control_sirens(struct execution_context_s *ctx, int value) {
	printf("Writing sound state: %d\n", value);
	post_write_command(ctx, "dev.485.rsrs2.state_sound1_on", value);
	post_write_command(ctx, "dev.485.rsrs2.state_sound1_vol", 0);
	post_write_command(ctx, "dev.485.rsrs2.state_sound1_led", value);
	post_write_command(ctx, "dev.485.rsrs2.state_sound1_rl", 0);
	post_write_command(ctx, "dev.485.rsrs2.state_sound2_on", value);
	post_write_command(ctx, "dev.485.rsrs2.state_sound2_vol", 0);
	post_write_command(ctx, "dev.485.rsrs2.state_sound2_led", value);
	post_write_command(ctx, "dev.485.rsrs2.state_sound2_rl", 0);
	post_process(ctx);
}

void process_sirens_timeout(int timeout_msec, volatile int *run_condition, struct execution_context_s *ctx) {
	struct timespec start, now;
	control_sirens(ctx, 1);
	
	clock_gettime(CLOCK_REALTIME, &start);
	while(*run_condition) {
		clock_gettime(CLOCK_REALTIME, &now);
		if((now.tv_sec > (start.tv_sec + 5)) || (now.tv_sec == (start.tv_sec + 5)) && (now.tv_nsec >= start.tv_nsec)) {
			printf("Exiting by timeout\n");
			break;
		}
		usleep(1000);
	}

	if(!(*run_condition)) {
		printf("Forcibly stop sirens!\n");
	}

	control_sirens(ctx, 0);
}

void post_command(logic_command_t cmd, struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	struct logic_command_s *command = malloc(sizeof(struct logic_command_s));

	command->command = cmd;
	command->signal = signal;
	command->value = value;

  ring_buffer_push(context->command_buffer, command);
	write(context->event_socket[1], "w", 1);
}

//char SignalHash[MAX_Signals][MAX_Signals]; //array for store index of Signals

int init(struct signal_s *signal, int value, struct execution_context_s *ctx){
  if(!signal_get(ctx, "dev.wago.oc_ready.state")) {
	  printf("Wago Ready error!\n");
	  write_journal(ST_WARNING,1001); //write to log system state and error kode wago
		return 0;
	}

	if(signal_get(ctx, "dev.wago.oc_mdi.err_phase")) {
		printf("Phase error!\n");
		write_journal(ST_WARNING,1); //write to log system state and error phase
		return 0;
	}

	if(signal_get(ctx, "dev.wago.bki_k1.M1") ||
		 signal_get(ctx, "dev.wago.bki_k2.M2") ||
		 signal_get(ctx, "dev.wago.bki_k3_k4.M3_M4") ||
		 signal_get(ctx, "dev.wago.bki_k5.M5") ||
		 signal_get(ctx, "dev.wago.bki_k7.M7"))
	{
		printf("BKI error!\n");
		write_journal(ST_WARNING,1); //write to log system state and error BKI
		return 0;
	}

	if(!signal_get(ctx, "dev.wago.oc_mdi1.oc_w_qf1")) {
		printf("No lever feedback\n");
		write_command(ctx, "dev.wago.oc_mdo1.ka7_1", 1);
		return 0;
	}

	printf("Initialization completed!\n");
	return 1;
}

int adc_to_hr(struct execution_context_s *ctx, char const *name, int value) {
  char step_name[64] = "";
  char shift_name[64] = "";
  snprintf(step_name, sizeof(step_name), "%s.step", name);
  snprintf(shift_name, sizeof(shift_name), "%s.shift", name);
  int mult = signal_get(ctx, step_name);
  int shift = signal_get(ctx, shift_name);
  if(mult == 0) {
    printf("Signal %s not found\n", step_name);
    return value;
  }
  return value / mult - shift;
}

int hr_to_adc(struct execution_context_s *ctx, char const *name, int value) {
  char step_name[64] = "";
  char shift_name[64] = "";
  snprintf(step_name, sizeof(step_name), "%s.step", name);
  snprintf(shift_name, sizeof(shift_name), "%s.shift", name);
  int mult = signal_get(ctx, step_name);
  int shift = signal_get(ctx, shift_name);
  if(mult == 0) {
    printf("Signal %s not found\n", step_name);
    return value;
  }
  return value * mult + shift;
}

int check_limits(struct execution_context_s *ctx, char *name, int value) {
  struct logic_context_s *lc = (struct logic_context_s*)ctx->clientstate;
  struct limit_s *limit = hash_find_first(lc->limits_hash, name);
  int state = LIMIT_NORM, t_react = 0;
	struct timespec now;
  int tflash = 5000;

  if(!limit) {
    printf("Creating limit for %s\n", name);
    limit = malloc(sizeof(struct limit_s));
    limit->state = 0;
    limit->level = 0;
    clock_gettime(CLOCK_REALTIME, &limit->start);
    limit->next = lc->limits;
    strcpy(limit->name, name);
    hash_add(lc->limits_hash, name, limit);
    lc->limits = limit;
  }

  char treact_name[64] = "";
  char min_name[64] = "";
  char max_name[64] = "";
  char min_warn_name[64] = "";
  char max_warn_name[64] = "";
  char enabled_min_name[64] = "";
  char enabled_max_name[64] = "";
  char enabled_min_warn_name[64] = "";
  char enabled_max_warn_name[64] = "";

  snprintf(treact_name, sizeof(min_name), "%s.t_react", name);
  snprintf(min_name, sizeof(min_name), "%s.min", name);
  snprintf(max_name, sizeof(max_name), "%s.max", name);
  snprintf(min_warn_name, sizeof(min_warn_name), "%s.warn.min", name);
  snprintf(max_warn_name, sizeof(max_warn_name), "%s.warn.max", name);
  snprintf(enabled_min_name, sizeof(enabled_min_name), "%s.enabled.min", name);
  snprintf(enabled_max_name, sizeof(enabled_max_name), "%s.enabled.max", name);
  snprintf(enabled_min_warn_name, sizeof(enabled_min_warn_name), "%s.enabled.warn.min", name);
  snprintf(enabled_max_warn_name, sizeof(enabled_max_warn_name), "%s.enabled.warn.max", name);

  int treact = signal_get(ctx, treact_name);
  int min = signal_get(ctx, min_name);
  int max = signal_get(ctx, max_name);
  int min_warn = signal_get(ctx, min_warn_name);
  int max_warn = signal_get(ctx, max_warn_name);
  int enabled_min = signal_get(ctx, enabled_min_name);
  int enabled_max = signal_get(ctx, enabled_max_name);
  int enabled_min_warn = signal_get(ctx, enabled_min_warn_name);
  int enabled_max_warn = signal_get(ctx, enabled_max_warn_name);

  int mv = adc_to_hr(ctx, name, value);

  if(mv <= min && enabled_min) {
    state = LIMIT_MIN_CRIT;
  }
  else if(mv <= min_warn && enabled_min_warn) {
    state = LIMIT_MIN_WARN;
  }
  else if(mv >= max && enabled_max) {
    state = LIMIT_MAX_CRIT;
  }
  else if(mv >= max_warn && enabled_max_warn) {
    state = LIMIT_MAX_WARN;
  }

  printf("Current state: %d\n", state);

  limit->level = value;

  if(limit->state != state) {
    // State changed
    printf("Current state changed to %d\n", state);
    clock_gettime(CLOCK_REALTIME, &limit->start);
    limit->state = state;
    if(treact != 0) {
      state = LIMIT_NORM;
    }
  } else if(state != LIMIT_NORM) {
    clock_gettime(CLOCK_REALTIME, &now);
    int diff_sec = now.tv_sec - limit->start.tv_sec;
    int diff_nsec = now.tv_nsec - limit->start.tv_nsec;
    int diff_msec = diff_sec * 1000 + (diff_nsec / 1000000);
    printf("TReact: %d; current time: %d\n", treact, diff_msec);

    if(diff_msec < treact) {
      printf("Skip limit due to reaction time\n");
      state = LIMIT_NORM;
    } else {
      printf("Applying limit\n");
      if((state == LIMIT_MIN_WARN || state == LIMIT_MAX_WARN) && ((diff_msec - treact) < tflash)) {
        // Enable fhash
        post_write_command(ctx, "dev.485.rsrs2.state_sound1_led", 1);
        post_write_command(ctx, "dev.485.rsrs2.state_sound2_led", 1);
      } else {
        post_write_command(ctx, "dev.485.rsrs2.state_sound1_led", 0);
        post_write_command(ctx, "dev.485.rsrs2.state_sound2_led", 0);
      }
    }
  }
  return state;
}
