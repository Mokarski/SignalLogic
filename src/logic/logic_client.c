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
