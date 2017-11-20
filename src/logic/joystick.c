#include <stdio.h>
#include <client/signalhelper.h>
#include <common/signal.h>
#include <client/clientcommand.h>
#include <logic/logic_client.h>
#include "local.h"
#include "joystick.h"
#include "processor.h"

#define MOVE_UP				0x1
#define MOVE_DOWN			0x2
#define MOVE_LEFT			0x4
#define MOVE_RIGHT		0x8

#define J_BIT_UP 			0
#define J_BIT_DOWN  	1

static void process_register_conv(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.pukonv485c.joy_left_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.pukonv485c.joy_down_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.pukonv485c.joy_right_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.pukonv485c.joy_up_conv", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.kei1.conveyor_left", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.kei1.conveyor_down", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.kei1.conveyor_right", &process_joystick_conv);
	processor_add(ctx, "dev.485.kb.kei1.conveyor_up", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_conv_left", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_conv_down", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_conv_right", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_conv_up", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_conv_left", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_conv_down", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_conv_right", &process_joystick_conv);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_conv_up", &process_joystick_conv);
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
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_forward", &process_joystick_move);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_back", &process_joystick_move);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_left", &process_joystick_move);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_right", &process_joystick_move);
}

static void process_register_execdev(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.exec_dev_left", &process_joystick_execdev);
	processor_add(ctx, "dev.485.kb.kei1.exec_dev_down", &process_joystick_execdev);
	processor_add(ctx, "dev.485.kb.kei1.exec_dev_right", &process_joystick_execdev);
	processor_add(ctx, "dev.485.kb.kei1.exec_dev_up", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_exec_dev_left", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_exec_dev_down", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_exec_dev_right", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485.kei.joy_exec_dev_up", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_exec_dev_left", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_exec_dev_down", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_exec_dev_right", &process_joystick_execdev);
	processor_add(ctx, "dev.485.rpdu485c.kei.joy_exec_dev_up", &process_joystick_execdev);
}

static void process_register_telescope(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.telescope_up", &process_joystick_telescope);
	processor_add(ctx, "dev.485.kb.kei1.telescope_down", &process_joystick_telescope);
	processor_add(ctx, "dev.485.rpdu485.kei.telescope_up", &process_joystick_telescope);
	processor_add(ctx, "dev.485.rpdu485.kei.telescope_down", &process_joystick_telescope);
	processor_add(ctx, "dev.485.rpdu485c.kei.telescope_up", &process_joystick_telescope);
	processor_add(ctx, "dev.485.rpdu485c.kei.telescope_down", &process_joystick_telescope);
}

static void process_register_support(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.combain_support_down", &process_joystick_support);
	processor_add(ctx, "dev.485.kb.kei1.combain_support_up", &process_joystick_support);
	processor_add(ctx, "dev.485.rpdu485.kei.support_down", &process_joystick_support);
	processor_add(ctx, "dev.485.rpdu485.kei.support_up", &process_joystick_support);
	processor_add(ctx, "dev.485.rpdu485c.kei.support_down", &process_joystick_support);
	processor_add(ctx, "dev.485.rpdu485c.kei.support_up", &process_joystick_support);
}

static void process_register_feeder(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.sourcer_down", &process_joystick_feeder);
	processor_add(ctx, "dev.485.kb.kei1.sourcer_up", &process_joystick_feeder);
	processor_add(ctx, "dev.485.rpdu485.kei.sourcer_down", &process_joystick_feeder);
	processor_add(ctx, "dev.485.rpdu485.kei.sourcer_up", &process_joystick_feeder);
	processor_add(ctx, "dev.485.rpdu485c.kei.sourcer_down", &process_joystick_feeder);
	processor_add(ctx, "dev.485.rpdu485c.kei.sourcer_up", &process_joystick_feeder);
}

void process_joystick_register(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.acceleration", &process_joystick_accel);
	processor_add(ctx, "dev.485.rpdu485.kei.acceleration_up", &process_joystick_accel);
	processor_add(ctx, "dev.485.rpdu485c.kei.acceleration_up", &process_joystick_accel);

	process_register_conv(ctx);
	process_register_move(ctx);
	process_register_feeder(ctx);
	process_register_execdev(ctx);
	process_register_support(ctx);
	process_register_telescope(ctx);
}

void process_joystick_conv(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int change_direction = 0;
	if(!is_oil_station_started(ctx)) return;
	if(control_mode(ctx) & LISTEN_CONV) {
		if(!strcmp(signal->s_name, "dev.485.kb.pukonv485c.joy_left_conv")) {
			change_direction = MOVE_LEFT;
		}
		if(!strcmp(signal->s_name, "dev.485.kb.pukonv485c.joy_down_conv")) {
			change_direction = MOVE_DOWN;
		}
		if(!strcmp(signal->s_name, "dev.485.kb.pukonv485c.joy_right_conv")) {
			change_direction = MOVE_RIGHT;
		}
		if(!strcmp(signal->s_name, "dev.485.kb.pukonv485c.joy_up_conv")) {
			change_direction = MOVE_UP;
		}
	} else if(control_mode(ctx) & LISTEN_LOCAL) {
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.conveyor_left")) {
			change_direction = MOVE_LEFT;
		}
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.conveyor_down")) {
			change_direction = MOVE_DOWN;
		}
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.conveyor_right")) {
			change_direction = MOVE_RIGHT;
		}
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.conveyor_up")) {
			change_direction = MOVE_UP;
		}
	} else if(control_mode(ctx) & LISTEN_RPDU) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.joy_conv_left")) {
			change_direction = MOVE_LEFT;
		}
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.joy_conv_down")) {
			change_direction = MOVE_DOWN;
		}
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.joy_conv_right")) {
			change_direction = MOVE_RIGHT;
		}
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.joy_conv_up")) {
			change_direction = MOVE_UP;
		}
	} else if(control_mode(ctx) & LISTEN_CABLE) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.joy_conv_left")) {
			change_direction = MOVE_LEFT;
		}
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.joy_conv_down")) {
			change_direction = MOVE_DOWN;
		}
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.joy_conv_right")) {
			change_direction = MOVE_RIGHT;
		}
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.joy_conv_up")) {
			change_direction = MOVE_UP;
		}
	}

	switch(change_direction) {
	case MOVE_UP:
		if(value) {
			write_command(ctx, "dev.485.rsrs.rm_u2_on3", !value);
			post_update_command(ctx, "dev.panel10.kb.kei1.conveyor_down", !value);
		}
		write_command(ctx, "dev.485.rsrs.rm_u2_on2", value);
		post_update_command(ctx, "dev.panel10.kb.kei1.conveyor_up", value);
		break;
	case MOVE_DOWN:
		if(value) {
			write_command(ctx, "dev.485.rsrs.rm_u2_on2", !value);
			post_update_command(ctx, "dev.panel10.kb.kei1.conveyor_up", !value);
		}
		write_command(ctx, "dev.485.rsrs.rm_u2_on3", value);
		post_update_command(ctx, "dev.panel10.kb.kei1.conveyor_down", value);
		break;
	case MOVE_LEFT:
		if(value) {
			write_command(ctx, "dev.485.rsrs.rm_u2_on4", !value);
			post_update_command(ctx, "dev.panel10.kb.kei1.conveyor_right", !value);
		}
		write_command(ctx, "dev.485.rsrs.rm_u2_on5", value);
		post_update_command(ctx, "dev.panel10.kb.kei1.conveyor_left", value);
		break;
	case MOVE_RIGHT:
		if(value) {
			write_command(ctx, "dev.485.rsrs.rm_u2_on5", !value);
			post_update_command(ctx, "dev.panel10.kb.kei1.conveyor_left", !value);
		}
		write_command(ctx, "dev.485.rsrs.rm_u2_on4", value);
		post_update_command(ctx, "dev.panel10.kb.kei1.conveyor_right", value);
		break;
	}
}

void process_joystick_move_change(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int jleft, jright, jup, jdown;
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	struct timespec now;
	int left_move = 0, right_move = 0;
	pthread_t self = pthread_self();

	printf("Processing truck joystick(s)\n");
	if(context->is_moving) {
		printf("Trucks are moving or starting\n");
		clock_gettime(CLOCK_REALTIME, &now);
		if((now.tv_sec - context->last_move.tv_sec > 6) && !context->trucks_started) {
			printf("Process sirenes\n");
			if(self != ctx->worker_thread) {
				post_command(&process_joystick_move_change, signal, value, ctx);
				return;
			}
			process_sirens_timeout(5, &context->is_moving, ctx);
		}

		if(!context->is_moving) {
			return;
		}

		context->trucks_started = 1;

		if(control_mode(ctx) & LISTEN_LOCAL) {
			left_move = (signal_get(ctx, "dev.485.kb.kei1.left_truck_back")   << J_BIT_DOWN) | (signal_get(ctx, "dev.485.kb.kei1.left_truck_forward")  << J_BIT_UP);
			right_move = (signal_get(ctx, "dev.485.kb.kei1.right_truck_back") << J_BIT_DOWN) | (signal_get(ctx, "dev.485.kb.kei1.right_truck_forward") << J_BIT_UP);
		} else if(control_mode(ctx) & LISTEN_RPDU) {
			jleft = signal_get(ctx, "dev.485.rpdu485.kei.joy_left");
			jright = signal_get(ctx, "dev.485.rpdu485.kei.joy_right");
			jup = signal_get(ctx, "dev.485.rpdu485.kei.joy_forward");
			jdown = signal_get(ctx, "dev.485.rpdu485.kei.joy_back");

			if(jup) {
				left_move = (jright << J_BIT_UP) | (!jleft << J_BIT_UP);
				right_move = (jleft << J_BIT_UP) | (!jright << J_BIT_UP);
			} else if(jdown) {
				left_move = (jright << J_BIT_DOWN) | (!jleft << J_BIT_DOWN);
				right_move = (jleft << J_BIT_DOWN) | (!jright << J_BIT_DOWN);
			} else {
				left_move = jright << J_BIT_UP | jleft << J_BIT_DOWN;
				right_move = jright << J_BIT_DOWN | jleft << J_BIT_UP;
			}
		} else if(control_mode(ctx) & LISTEN_CABLE) {
			jleft = signal_get(ctx, "dev.485.rpdu485c.kei.joy_left");
			jright = signal_get(ctx, "dev.485.rpdu485c.kei.joy_right");
			jup = signal_get(ctx, "dev.485.rpdu485c.kei.joy_forward");
			jdown = signal_get(ctx, "dev.485.rpdu485c.kei.joy_back");

			if(jup) {
				left_move = (jright << J_BIT_UP) | (!jleft << J_BIT_UP);
				right_move = (jleft << J_BIT_UP) | (!jright << J_BIT_UP);
			} else if(jdown) {
				left_move = (jright << J_BIT_DOWN) | (!jleft << J_BIT_DOWN);
				right_move = (jleft << J_BIT_DOWN) | (!jright << J_BIT_DOWN);
			} else {
				left_move = jright << J_BIT_UP | jleft << J_BIT_DOWN;
				right_move = jright << J_BIT_DOWN | jleft << J_BIT_UP;
			}
		}

		if(left_move & (1 << J_BIT_UP)) {
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on11", 0);
			post_update_command(ctx, "dev.panel10.kb.kei2.left_truck_back", 0);
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on10", 1);
			post_update_command(ctx, "dev.panel10.kb.kei2.left_truck_forward", 1);
		} else if(left_move & (1 << J_BIT_DOWN)) {
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on10", 0);
			post_update_command(ctx, "dev.panel10.kb.kei2.left_truck_forward", 0);
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on11", 1);
			post_update_command(ctx, "dev.panel10.kb.kei2.left_truck_back", 1);
		} else {
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on10", 0);
			post_update_command(ctx, "dev.panel10.kb.kei2.left_truck_forward", 0);
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on11", 0);
			post_update_command(ctx, "dev.panel10.kb.kei2.left_truck_back", 0);
		}
		if(right_move & (1 << J_BIT_UP)) {
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on1", 0);
			post_update_command(ctx, "dev.panel10.kb.kei2.right_truck_back", 0);
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on0", 1);
			post_update_command(ctx, "dev.panel10.kb.kei2.right_truck_forward", 1);
		} else if(right_move & (1 << J_BIT_DOWN)) {
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on0", 0);
			post_update_command(ctx, "dev.panel10.kb.kei2.right_truck_forward", 0);
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on1", 1);
			post_update_command(ctx, "dev.panel10.kb.kei2.right_truck_back", 1);
		} else {
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on0", 0);
			post_update_command(ctx, "dev.panel10.kb.kei2.right_truck_forward", 0);
			post_write_command(ctx, "dev.485.rsrs.rm_u2_on1", 0);
			post_update_command(ctx, "dev.panel10.kb.kei2.right_truck_back", 0);
		}
		if(!left_move && !right_move) {
			clock_gettime(CLOCK_REALTIME, (struct timespec*)&context->last_move);
			context->trucks_started = 0;
		}
	} else {
		printf("Trucks are stopping\n");
		if(context->trucks_started)
			clock_gettime(CLOCK_REALTIME, (struct timespec*)&context->last_move);
		post_write_command(ctx, "dev.485.rsrs.rm_u2_on10", 0);
		post_update_command(ctx, "dev.panel10.kb.kei2.left_truck_forward", 0);
		post_write_command(ctx, "dev.485.rsrs.rm_u2_on11", 0);
		post_update_command(ctx, "dev.panel10.kb.kei2.left_truck_back", 0);
		post_write_command(ctx, "dev.485.rsrs.rm_u2_on0", 0);
		post_update_command(ctx, "dev.panel10.kb.kei2.right_truck_forward", 0);
		post_write_command(ctx, "dev.485.rsrs.rm_u2_on1", 0);
		post_update_command(ctx, "dev.panel10.kb.kei2.right_truck_back", 0);
		context->trucks_started = 0;
	}
}

void process_joystick_move(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!is_oil_station_started(ctx)) return;
	signal->s_value = value;
	int is_moving = 
		signal_get(ctx, "dev.485.kb.kei1.left_truck_back") || signal_get(ctx, "dev.485.kb.kei1.left_truck_forward") ||
		signal_get(ctx, "dev.485.kb.kei1.right_truck_back") || signal_get(ctx, "dev.485.kb.kei1.right_truck_forward") ||
		signal_get(ctx, "dev.485.rpdu485.kei.joy_forward") || signal_get(ctx, "dev.485.rpdu485.kei.joy_back") ||
		signal_get(ctx, "dev.485.rpdu485.kei.joy_left") || signal_get(ctx, "dev.485.rpdu485.kei.joy_right") ||
		signal_get(ctx, "dev.485.rpdu485c.kei.joy_forward") || signal_get(ctx, "dev.485.rpdu485c.kei.joy_back") ||
		signal_get(ctx, "dev.485.rpdu485c.kei.joy_left") || signal_get(ctx, "dev.485.rpdu485c.kei.joy_right");

	if(is_moving) {
		context->is_moving = is_moving;
	} else {
		// Update last moving time
		if(context->is_moving) {
			context->is_moving = is_moving;
		}
	}
	process_joystick_move_change(signal, value, ctx);
}

void process_joystick_execdev(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int change_direction = 0;
	if(!is_oil_station_started(ctx)) return;
	if(control_mode(ctx) & LISTEN_LOCAL) {
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.exec_dev_left"))
			change_direction = MOVE_LEFT;
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.exec_dev_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.exec_dev_right"))
			change_direction = MOVE_RIGHT;
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.exec_dev_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_RPDU) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.joy_exec_dev_left"))
			change_direction = MOVE_LEFT;
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.joy_exec_dev_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.joy_exec_dev_right"))
			change_direction = MOVE_RIGHT;
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.joy_exec_dev_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_CABLE) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.joy_exec_dev_left"))
			change_direction = MOVE_LEFT;
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.joy_exec_dev_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.joy_exec_dev_right"))
			change_direction = MOVE_RIGHT;
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.joy_exec_dev_up"))
			change_direction = MOVE_UP;
	}

	switch(change_direction) {
	case MOVE_UP:
		write_command(ctx, "dev.485.rsrs.rm_u1_on6", value);
		post_update_command(ctx, "dev.panel10.kb.kei3.exec_dev_up", value);
		break;
	case MOVE_DOWN:
		write_command(ctx, "dev.485.rsrs.rm_u1_on7", value);
		post_update_command(ctx, "dev.panel10.kb.kei2.exec_dev_down", value);
		break;
	case MOVE_LEFT:
		write_command(ctx, "dev.485.rsrs.rm_u1_on3", value);
		post_update_command(ctx, "dev.panel10.kb.kei2.exec_dev_left", value);
		break;
	case MOVE_RIGHT:
		write_command(ctx, "dev.485.rsrs.rm_u1_on2", value);
		post_update_command(ctx, "dev.panel10.kb.kei2.exec_dev_right", value);
		break;
	}
}

void process_joystick_telescope(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int change_direction = 0;
	if(!is_oil_station_started(ctx)) return;
	if(control_mode(ctx) & LISTEN_LOCAL) {
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.telescope_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.telescope_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_RPDU) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.telescope_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.telescope_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_CABLE) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.telescope_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.telescope_up"))
			change_direction = MOVE_UP;
	}

	switch(change_direction) {
	case MOVE_UP:
		write_command(ctx, "dev.485.rsrs.rm_u1_on4", value);
		post_update_command(ctx, "dev.panel10.kb.kei2.telescope_up", value);
		break;
	case MOVE_DOWN:
		write_command(ctx, "dev.485.rsrs.rm_u1_on5", value);
		post_update_command(ctx, "dev.panel10.kb.kei3.telescope_down", value);
		break;
	}
}

void process_joystick_support(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int change_direction = 0;
	if(!is_oil_station_started(ctx)) return;
	if(control_mode(ctx) & LISTEN_LOCAL) {
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.combain_support_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.combain_support_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_RPDU) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.support_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.support_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_CABLE) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.support_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.support_up"))
			change_direction = MOVE_UP;
	}

	switch(change_direction) {
	case MOVE_UP:
		write_command(ctx, "dev.485.rsrs.rm_u2_on8", value);
		post_update_command(ctx, "dev.panel10.kb.kei2.combain_support_up", value);
		break;
	case MOVE_DOWN:
		write_command(ctx, "dev.485.rsrs.rm_u2_on9", value);
		post_update_command(ctx, "dev.panel10.kb.kei1.combain_support_down", value);
		break;
	}
}

void process_joystick_accel(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int change_direction = 0;
	if(!is_oil_station_started(ctx)) return;
	if(control_mode(ctx) & LISTEN_LOCAL) {
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.acceleration"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_RPDU) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.acceleration_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_CABLE) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.acceleration_up"))
			change_direction = MOVE_UP;
	}

	switch(change_direction) {
	case MOVE_UP:
		write_command(ctx, "dev.485.rsrs.rm_u1_on0", value);
		break;
	}
}

void process_joystick_feeder(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	int change_direction = 0;
	if(!is_oil_station_started(ctx)) return;
	if(control_mode(ctx) & LISTEN_LOCAL) {
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.sourcer_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.kb.kei1.sourcer_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_RPDU) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.sourcer_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.sourcer_up"))
			change_direction = MOVE_UP;
	} else if(control_mode(ctx) & LISTEN_CABLE) {
		if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.sourcer_down"))
			change_direction = MOVE_DOWN;
		if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.sourcer_up"))
			change_direction = MOVE_UP;
	}

	switch(change_direction) {
	case MOVE_UP:
		write_command(ctx, "dev.485.rsrs.rm_u1_on8", value);
		post_update_command(ctx, "dev.panel10.kb.kei1.sourcer_up", value);
		break;
	case MOVE_DOWN:
		write_command(ctx, "dev.485.rsrs.rm_u1_on9", value);
		post_update_command(ctx, "dev.panel10.kb.kei1.sourcer_down", value);
		break;
	}

}
