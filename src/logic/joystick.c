#include <client/signalhelper.h>
#include "joystick.h"
#include "processor.h"

#define LISTEN_NONE		0x0
#define LISTEN_LOCAL	0x1
#define LISTEN_RPDU		0x2
#define LISTEN_CONV		0x4
#define LISTEN_CABLE	0x8
#define LISTEN_MAIN		(LISTEN_LOCAL | LISTEN_RPDU | LISTEN_CABLE)

#define CONTROL_MASK		0x3
#define CONTROL_RADIO		0x1
#define	CONTROL_MANU		0x2
#define CONTROL_CABLE		0x3

#define MODE_MASK				(0x03 << 2)
#define MODE_DIAG				(0x01 << 2)
#define MODE_PUMP				(0x02 << 2)
#define MODE_NORM				(0x03 << 2)

#define MOVE_UP				0x1
#define MOVE_DOWN			0x2
#define MOVE_LEFT			0x4
#define MOVE_RIGHT		0x8

static int control_mode = LISTEN_LOCAL;

static void process_register_conv(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.pukonv485c.joy_left_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.pukonv485c.joy_down_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.pukonv485c.joy_right_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.pukonv485c.joy_up_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.kei1.conveyor_left", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.kei1.conveyor_down", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.kei1.conveyor_right", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.kei1.conveyor_up", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_left_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_down_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_right_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_up_conv", &process_joystick_conv);
}

static void process_register_move(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.left_truck_back", &process_joystick_move);
	processor_add(ctx, "dev.485.kb.kei1.left_truck_forward", &process_joystick_move);
	processor_add(ctx, "dev.485.kb.kei1.right_truck_back", &process_joystick_move);
	processor_add(ctx, "dev.485.kb.kei1.right_truck_forward", &process_joystick_move);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_forward", &process_joystick_move);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_back", &process_joystick_move);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_left", &process_joystick_move);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_right", &process_joystick_move);
}

static void process_register_execdev(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.exec_dev_left", &process_joystick_execdev);
	processor_add(ctx, "dev.485.kb.kei1.exec_dev_down", &process_joystick_execdev);
	processor_add(ctx, "dev.485.kb.kei1.exec_dev_left", &process_joystick_execdev);
	processor_add(ctx, "dev.485.kb.kei1.exec_dev_up", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_exec_dev_left", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_exec_dev_down", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_exec_dev_right", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_exec_dev_up", &process_joystick_execdev);
}

static void process_register_telescope(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.telescope_up", &process_joystick_telescope);
	processor_add(ctx, "dev.485.kb.kei1.telescope_down", &process_joystick_telescope);
	processor_add(ctx, "dev.485.rpdu485.kei.telescope_up", &process_joystick_telescope);
	processor_add(ctx, "dev.485.rpdu485.kei.telescope_down", &process_joystick_telescope);
}

static void process_register_support(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.combain_support_down", &process_joystick_support);
	processor_add(ctx, "dev.485.kb.kei1.combain_support_up", &process_joystick_support);
	processor_add(ctx, "dev.485.rpdu485.kei.support_down", &process_joystick_support);
	processor_add(ctx, "dev.485.rpdu485.kei.support_up", &process_joystick_support);
}

static void process_register_feeder(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.sourcer_down", &process_joystick_feeder);
	processor_add(ctx, "dev.485.kb.kei1.sourcer_up", &process_joystick_feeder);
	processor_add(ctx, "dev.485.rpdu485.kei.sourcer_down", &process_joystick_feeder);
	processor_add(ctx, "dev.485.rpdu485.kei.sourcer_up", &process_joystick_feeder);
}

void process_joystick_register(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.mode1", &process_mode_switch);
	processor_add(ctx, "dev.485.kb.kei1.mode2", &process_mode_switch);
	processor_add(ctx, "dev.485.kb.kei1.control1", &process_mode_switch);
	processor_add(ctx, "dev.485.kb.kei1.control2", &process_mode_switch);

	process_register_conv(ctx);
	process_register_move(ctx);
	process_register_feeder(ctx);
	process_register_execdev(ctx);
	process_register_support(ctx);
	process_register_telescope(ctx);
}

#define MOVE_UP				0x1
#define MOVE_DOWN			0x2
#define MOVE_LEFT			0x4
#define MOVE_RIGHT		0x8
void process_joystick_conv(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int change_direction = 0;
	if(control_mode & LISTEN_CONV) {
		if(strcmp(signal->s_name, ""))
		return;
	} else {
	}
}

void process_joystick_move(struct signal_s *signal, int value, struct execution_context_s *ctx) {
}

void process_joystick_execdev(struct signal_s *signal, int value, struct execution_context_s *ctx) {
}

void process_joystick_telescope(struct signal_s *signal, int value, struct execution_context_s *ctx) {
}

void process_joystick_support(struct signal_s *signal, int value, struct execution_context_s *ctx) {
}

void process_joystick_feeder(struct signal_s *signal, int value, struct execution_context_s *ctx) {
}

void process_mode_switch(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int mode1 = signal_get(ctx, "dev.485.kb.kei1.mode1");
	int mode2 = signal_get(ctx, "dev.485.kb.kei1.mode2");
	int control1 = signal_get(ctx, "dev.485.kb.kei1.control1");
	int control2 = signal_get(ctx, "dev.485.kb.kei1.control2");
	int post_conv = signal_get(ctx, "dev.485.kb.kei1.post_conveyor");
	int state = control1 | (control2 << 1) | (mode1 << 2) | (mode2 << 3);

	if(state & MODE_MASK == MODE_PUMP) {
		control_mode = LISTEN_NONE;
		return;
	}

	switch(state & CONTROL_MASK) {
	case CONTROL_RADIO:
		control_mode = LISTEN_RPDU;
		break;
	case CONTROL_MANU:
		control_mode = LISTEN_LOCAL;
		break;
	case CONTROL_CABLE:
		control_mode = LISTEN_CABLE;
		break;
	}

	if(post_conv) {
		control_mode |= LISTEN_CONV;
	}
}
