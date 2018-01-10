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
#ifdef MODBUS_ENABLE
#include <modbus.h>
#endif
#include "common/signal.h"
#include "common/subscription.h"
#include "common/ringbuffer.h"
#include "client/client.h"
#include "client/signalhelper.h"
#include "mbdev.h"

struct modbus_client_s {
	void *mb_context;
	struct mb_device_list_s *dev_list;
	struct execution_context_s *exec_ctx;
	struct timespec last_write;
};

int modbus_read(struct mb_device_list_s *ctx, int mbid, int max_regs) {
#ifdef MODBUS_ENABLE
	struct modbus_client_s *client = (struct modbus_client_s *)((struct execution_context_s*)ctx->mb_context)->clientstate;
  modbus_t *mb_ctx = client->mb_context;
  uint16_t regs[MAX_REG];
  int i;

  usleep(100);

  if(mb_ctx != NULL) {
    modbus_set_slave(mb_ctx, mbid);
    int connected = modbus_connect(mb_ctx);
    if(connected < 0) {
      modbus_flush(mb_ctx);
      modbus_close(mb_ctx);
      return -1;
    }

    int rc = modbus_read_registers(mb_ctx, 0, max_regs, regs);
     
		//printf("MB_Read device %d:%d \n", mbid, max_regs);
    modbus_flush(mb_ctx);
    modbus_close(mb_ctx);

    if(rc == -1) {
      return -1;
    }

    for(i = 0; i < max_regs; i ++) {
      ctx->device[mbid].reg[i].value = regs[i];
    }

    return 0;
  } else {
    //printf("Read: No modbus device opened\n");
    return -1;
  }
#else // Testing without modbus library 
  usleep(10000);
  return 0;
#endif
}

int modbus_write(struct mb_device_list_s *ctx, int mbid, int reg, int value) {
#ifdef MODBUS_ENABLE
	struct modbus_client_s *client = (struct modbus_client_s *)((struct execution_context_s*)ctx->mb_context)->clientstate;
  modbus_t *mb_ctx = client->mb_context;
  uint16_t regs[MAX_REG];
	unsigned long long interim;
	struct timespec now;

	clock_gettime(CLOCK_REALTIME, &now);
	interim = (now.tv_sec - client->last_write.tv_sec) * 1000;
	interim += now.tv_nsec / 1000000;
	interim -= client->last_write.tv_nsec / 1000000;

	printf("Last write was performed %llu msec ago\n", interim);

	if(interim < 15) {
		usleep((15-interim) * 1000);
	}

	clock_gettime(CLOCK_REALTIME, &client->last_write);
  //printf("Writing register %d:%d, %x\n", mbid, reg, value);
  if(mb_ctx != NULL) {
    modbus_set_slave(mb_ctx, mbid);
    int connected = modbus_connect(mb_ctx);
    if(connected < 0) {
			printf("Writing modbus register failed: couldn't connect\n");
      modbus_flush(mb_ctx);
      modbus_close(mb_ctx);
      return -1;
    }

		uint16_t registers[1] = { value };
		int rc = modbus_write_registers(mb_ctx, reg, 1, registers); //write in device by register
		printf("Write device %d:%d value %d\n", mbid, reg, value);

    modbus_flush(mb_ctx);
    modbus_close(mb_ctx);

    if(rc == -1) {
			printf("Writing modbus register failed: write failed\n");
      return -1;
    }

		usleep(2000);
    return 0;
  } else {
    printf("Write: No modbus device opened\n");
    return -1;
  }
#else // Testing without modbus library 
  usleep(10000);
  printf("Writing device %d:%d: %d\n", mbid, reg, value);
  return 0;
#endif
}

void event_update_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  // Should never get here
  printf("*** EVENT HANDLER: Signal %s updating from value %d to value %d\n", signal->s_name, signal->s_value, value);
}

void event_write_signal(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct modbus_client_s *client = (struct modbus_client_s *)ctx->clientstate;
  mb_dev_add_write_request(client->dev_list, signal, value);
}

void *create_mb_context() {
#ifdef MODBUS_ENABLE
  modbus_t *ctx = modbus_new_rtu ("/dev/ttySP0", 115200, 'N', 8, 1);

  if(!ctx) {
    perror("Couldn't open RTU modbus\n");
  }

	modbus_set_error_recovery(ctx, MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);
	modbus_set_debug(ctx, 0);

  struct timeval byte_timeout;
	struct timeval response_timeout;

  /* Define a new and too short timeout! */
	response_timeout.tv_sec = 0;
	response_timeout.tv_usec = 15000;
	modbus_set_response_timeout(ctx, &response_timeout);

  //byte_timeout.tv_sec = 0;
  //byte_timeout.tv_usec = 10;
  //modbus_set_byte_timeout(ctx, &byte_timeout);
	return ctx;
#else
  return NULL;
#endif
}

int modbus_signal_error(struct mb_device_list_s *ctx, struct signal_s *signal, int error) {
	if(error) printf("Error signal %s\n", signal->s_name);
}

int modbus_signal_updated(struct mb_device_list_s *ctx, struct signal_s *signal) {
	int mbid = signal->s_register.dr_device.d_mb_id;
	int reg = signal->s_register.dr_addr;

	if(strstr(signal->s_name,  "dev.485.ad") != NULL)
		printf("Posting update %s : %d; register: %d; bit %d\n", signal->s_name, signal->s_value, ctx->device[mbid].reg[reg].value, signal->s_register.dr_bit);

	post_update_command(ctx->mb_context, signal->s_name, signal->s_value);
	post_process(ctx->mb_context);
}

void client_init(struct execution_context_s *ctx, int argc, char **argv) {
  struct signal_s *s;
  struct mb_device_list_s *dlist;
	struct modbus_client_s *client;

  dlist = malloc(sizeof(struct mb_device_list_s));
  mb_dev_list_init(dlist);

  client = malloc(sizeof(struct modbus_client_s));
  client->mb_context = create_mb_context();
  client->dev_list = dlist;
  client->exec_ctx = ctx;

  dlist->mb_read_device  = &modbus_read;
  dlist->mb_write_device = &modbus_write;
	dlist->mb_signal_updated = &modbus_signal_updated;
	dlist->mb_signal_error = &modbus_signal_error;
  dlist->mb_context = ctx;

  ctx->clientstate = client;
  get_and_subscribe(ctx, "dev.485", SUB_WRITE);
  s = ctx->signals;
  while(s) {
    mb_dev_add_signal(dlist, s, 0);
    s = s->next;
  }
}

// Polling modbus devices
void client_thread_proc(struct execution_context_s *ctx) {
	struct modbus_client_s *client = (struct modbus_client_s *)ctx->clientstate;
  printf("Started modbus proc thread\n");
	printf("Signals: %p\n", ctx->signals);
	clock_gettime(CLOCK_REALTIME, &client->last_write);
  while(ctx->running) {
    mb_dev_update(client->dev_list);
    post_process(ctx);
  }
}
