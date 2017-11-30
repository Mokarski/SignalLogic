#pragma once
#include <pthread.h>
#include <client/client.h>
#include <common/signal.h>
#include <common/hash.h>
#include <common/ringbuffer.h>

#define LIMIT_NORM      0
#define LIMIT_MIN_WARN  -1
#define LIMIT_MIN_CRIT  -2
#define LIMIT_MAX_WARN  1
#define LIMIT_MAX_CRIT  2

typedef void (*logic_command_t)(struct signal_s *signal, int value, struct execution_context_s *ctx);

struct logic_command_s {
	logic_command_t command;
	struct signal_s *signal;
	int value;
};

struct limit_s {
	struct timespec start;
	struct timespec start_flash;
  char name[64];
  int state;
  int level;
  struct limit_s *next;
};

struct logic_context_s {
	struct hash_s  *proc_hash;
	struct ring_buffer_s *command_buffer;
	volatile struct timespec last_move;
	volatile struct timespec last_sirens;
	volatile int 	  initialized;
	volatile int 	  diagnostic;
	volatile int 	  is_moving;
	volatile int 	  trucks_started;
	volatile int 	  oil_station_running;
	volatile int 		control_mode;
	volatile int 		function_mode;
	volatile int 		stars;
	volatile int 		sirenes;
	volatile int 		in_progress[10];
	int    					event_socket[2];
  struct limit_s *limits;
	struct hash_s  *limits_hash;
  pthread_mutex_t limits_mutex;
};

int  is_oil_station_started(struct execution_context_s *ctx);
void control_sirens(struct execution_context_s *ctx, int value);
void process_sirens_timeout(int timeout_msec, volatile int *run_condition, struct execution_context_s *ctx);
void post_command(logic_command_t, struct signal_s *signal, int value, struct execution_context_s *ctx);
int  adc_to_hr(struct execution_context_s *ctx, char const *name, int value);
int  hr_to_adc(struct execution_context_s *ctx, char const *name, int value);
int  check_limits(struct execution_context_s *ctx, char *name, int value);
int  stop_check_limits(struct execution_context_s *ctx, char *name);
