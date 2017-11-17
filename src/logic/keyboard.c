#include "keyboard.h"
#include "../client/client.h"
#include "../client/clientcommand.h"
#include "../client/signalhelper.h"

#define __USE_UNIX98
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>

#define MODE_IDLE			0
#define MODE_PUMPING	1
#define MODE_DIAG			2
#define MODE_NORM			3

#define IS_UP(j)	((joystick[j] & JOYVAL_UP))
#define IS_DOWN(j)	((joystick[j] & JOYVAL_DOWN))
#define IS_LEFT(j)	((joystick[j] & JOYVAL_LEFT))
#define IS_RIGHT(j)	((joystick[j] & JOYVAL_RIGHT))

pthread_mutex_t g_waitMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_waitCond = PTHREAD_COND_INITIALIZER;
pthread_t g_worker;
static volatile int g_mode = 0, g_workStarted = 0, g_diag = 0;
volatile int buttons[32] = {0};
volatile int joystick[32] = {0};

void process_register_common(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.sound_alarm", &process_sirenes);
	processor_add(ctx, "dev.485.kb.pukonv485c.beep",  &process_sirenes);
	processor_add(ctx, "dev.485.rpdu485.kei.sound_beepl", &process_sirenes);

	processor_add(ctx, "dev.485.kb.kei1.stop_alarm", &process_urgent_stop);
	processor_add(ctx, "dev.485.rpdu485.kei.crit_stop", &process_urgent_stop);
	processor_add(ctx, "dev.485.kb.pukonv485c.stop_alarm", &process_urgent_stop);
}

void process_urgent_stop(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	stop_all(signal, value, ctx);
	stop_Hydraulics(signal, value, ctx);
	control_sirens(ctx, 0);
}

void process_sirenes(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	control_sirens(ctx, value);
}

int Get_Signal(char *name) {
	return signal_get(g_Ctx, name);
}
