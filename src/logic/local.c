#include <stdio.h>
#include <client/signalhelper.h>
#include <common/signal.h>
#include <client/clientcommand.h>
#include <logic/logic_client.h>
#include "local.h"
#include "process.h"
#include "processor.h"

#define CONTROL_MASK		0x3
#define CONTROL_RADIO		0x1
#define	CONTROL_MANU		0x2
#define CONTROL_CABLE		0x3

#define STARS_FWD		1
#define STARS_REV		2

static void process_mode_switch(struct signal_s *signal, int value, struct execution_context_s *ctx);
static void process_rpdu_state(struct signal_s *signal, int value, struct execution_context_s *ctx);
static void process_rpduc_state(struct signal_s *signal, int value, struct execution_context_s *ctx);

void process_local_post_switch_register(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.kei1.mode1", &process_mode_switch);
	processor_add(ctx, "dev.485.kb.kei1.mode2", &process_mode_switch);
	processor_add(ctx, "dev.485.kb.kei1.control1", &process_mode_switch);
	processor_add(ctx, "dev.485.kb.kei1.control2", &process_mode_switch);
	processor_add(ctx, "dev.485.kb.kei1.post_conveyor", &process_mode_switch);
}

void process_local_post_register(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.kb.key.start_oil_pump", &process_local_pumping);
	processor_add(ctx, "dev.485.kb.key.stop_oil_pump", &process_local_pumping);

	processor_add(ctx, "dev.485.kb.key.start_oil_station", &process_local_oil_station);
	processor_add(ctx, "dev.485.kb.key.stop_oil_station", &process_local_oil_station);
	processor_add(ctx, "dev.485.rpdu485.kei.oil_station_up", &process_local_oil_station);
	processor_add(ctx, "dev.485.rpdu485.kei.oil_station_down", &process_local_oil_station);
	processor_add(ctx, "dev.485.rpdu485c.kei.oil_station_up", &process_local_oil_station);
	processor_add(ctx, "dev.485.rpdu485c.kei.oil_station_down", &process_local_oil_station);

	processor_add(ctx, "dev.485.kb.key.start_hydratation", &process_local_hydratation);
	processor_add(ctx, "dev.485.kb.key.stop_hydratation", &process_local_hydratation);

	processor_add(ctx, "dev.485.kb.key.start_exec_dev", &process_local_exec);
	processor_add(ctx, "dev.485.kb.key.stop_exec_dev", &process_local_exec);
	processor_add(ctx, "dev.485.rpdu485.kei.exec_dev_up", &process_local_exec);
	processor_add(ctx, "dev.485.rpdu485.kei.exec_dev_down", &process_local_exec);
	processor_add(ctx, "dev.485.rpdu485c.kei.exec_dev_up", &process_local_exec);
	processor_add(ctx, "dev.485.rpdu485c.kei.exec_dev_down", &process_local_exec);

	processor_add(ctx, "dev.485.kb.key.start_conveyor", &process_local_conveyor);
	processor_add(ctx, "dev.485.kb.key.stop_conveyor", &process_local_conveyor);
	processor_add(ctx, "dev.485.rpdu485.kei.conveyor_up", &process_local_conveyor);
	processor_add(ctx, "dev.485.rpdu485.kei.conveyor_down", &process_local_conveyor);
	processor_add(ctx, "dev.485.rpdu485c.kei.conveyor_up", &process_local_conveyor);
	processor_add(ctx, "dev.485.rpdu485c.kei.conveyor_down", &process_local_conveyor);



	processor_add(ctx, "dev.485.kb.key.start_stars", &process_local_stars);
	processor_add(ctx, "dev.485.kb.key.stop_stars", &process_local_stars);
	processor_add(ctx, "dev.485.rpdu485.kei.loader_up", &process_local_stars);
	processor_add(ctx, "dev.485.rpdu485.kei.loader_down", &process_local_stars);
	processor_add(ctx, "dev.485.rpdu485c.kei.loader_up", &process_local_stars);
	processor_add(ctx, "dev.485.rpdu485c.kei.loader_down", &process_local_stars);

	processor_add(ctx, "dev.485.rpdu485.kei.stop_loader", &process_local_stop_loading);
	processor_add(ctx, "dev.485.rpdu485c.kei.stop_loader", &process_local_stop_loading);
	processor_add(ctx, "dev.485.kb.pukonv485c.stop_loader", &process_local_stop_loading);

	processor_add(ctx, "dev.485.kb.key.start_reloader", &process_local_reloader);
	processor_add(ctx, "dev.485.kb.key.stop_reloader", &process_local_reloader);
	processor_add(ctx, "dev.485.rpdu485.kei.reloader_up", &process_local_reloader);
	processor_add(ctx, "dev.485.rpdu485.kei.reloader_down", &process_local_reloader);
	processor_add(ctx, "dev.485.rpdu485c.kei.reloader_up", &process_local_reloader);
	processor_add(ctx, "dev.485.rpdu485c.kei.reloader_down", &process_local_reloader);

	processor_add(ctx, "dev.485.kb.kei1.start_all", &process_local_all);
	processor_add(ctx, "dev.485.kb.kei1.stop_all", &process_local_all);
	processor_add(ctx, "dev.485.rpdu485.kei.start_all", &process_local_all);
	processor_add(ctx, "dev.485.rpdu485.kei.stop_all", &process_local_all);
	processor_add(ctx, "dev.485.rpdu485c.kei.start_all", &process_local_all);
	processor_add(ctx, "dev.485.rpdu485c.kei.stop_all", &process_local_all);

	processor_add(ctx, "dev.485.rpdu485.connect", &process_rpdu_state);
	processor_add(ctx, "dev.485.rpdu485c.connect", &process_rpduc_state);

  process_mode_switch(NULL, 0, ctx);
}

static void process_rpdu_state(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(value == 1) {
		post_write_command(ctx, "dev.485.rpdu485.kbl.reloader_green", context->in_progress[OVERLOADING] != 0);
		post_write_command(ctx, "dev.485.rpdu485.kbl.conveyor_green", context->in_progress[CONVEYOR] != 0);
		post_write_command(ctx, "dev.485.rpdu485.kbl.loader_green", context->in_progress[STARS] != 0);
		post_write_command(ctx, "dev.485.rpdu485.kbl.oil_station_green", context->in_progress[OIL] != 0);
		post_write_command(ctx, "dev.485.rpdu485.kbl.exec_dev_green", context->in_progress[ORGAN] != 0);
	}
	post_update_command(ctx, "dev.panel10.system_radio", value);
  //process_mode_switch(signal, value, ctx);

  // Disconnected - listen from local post
  if(!value && (control_mode(ctx) & LISTEN_RPDU)) {
		context->control_mode = context->control_mode & ~LISTEN_RPDU;
		context->control_mode |= LISTEN_LOCAL;
  }
}

static void process_rpduc_state(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(value == 1) {
		post_write_command(ctx, "dev.485.rpdu485c.kbl.reloader_green", context->in_progress[OVERLOADING] != 0);
		post_write_command(ctx, "dev.485.rpdu485c.kbl.conveyor_green", context->in_progress[CONVEYOR] != 0);
		post_write_command(ctx, "dev.485.rpdu485c.kbl.loader_green", context->in_progress[STARS] != 0);
		post_write_command(ctx, "dev.485.rpdu485c.kbl.oil_station_green", context->in_progress[OIL] != 0);
		post_write_command(ctx, "dev.485.rpdu485c.kbl.exec_dev_green", context->in_progress[ORGAN] != 0);
	}
  //process_mode_switch(signal, value, ctx);

  // Disconnected - listen from local post
  if(!value && (control_mode(ctx) & LISTEN_CABLE)) {
		context->control_mode = context->control_mode & ~LISTEN_CABLE;
		context->control_mode |= LISTEN_LOCAL;
  }
}

int control_mode(struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	return context->control_mode;
}

int function_mode(struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	return context->function_mode;
}

void process_local_reloader(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!value || function_mode(ctx) == MODE_PUMP) return;
	if(!strcmp(signal->s_name, "dev.485.kb.key.start_reloader")) {
		if(control_mode(ctx) & LISTEN_LOCAL) {
			post_command(&start_Overloading, signal, value, ctx);
		}
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.reloader_up")) {
		if(control_mode(ctx) & LISTEN_RPDU) {
			post_command(&start_Overloading, signal, value, ctx);
		}
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.reloader_up")) {
		if(control_mode(ctx) & LISTEN_CABLE) {
			post_command(&start_Overloading, signal, value, ctx);
		}
		return;
	}

	stop_Overloading(signal, value, ctx);
}

void process_local_conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!value || function_mode(ctx) == MODE_PUMP) return;
	if(!strcmp(signal->s_name, "dev.485.kb.key.start_conveyor")) {
		if(control_mode(ctx) & LISTEN_LOCAL) {
			post_command(&start_Conveyor, signal, value, ctx);
		}
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.conveyor_up")) {
		if(control_mode(ctx) & LISTEN_RPDU) {
			post_command(&start_Conveyor, signal, value, ctx);
		}
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.conveyor_up")) {
		if(control_mode(ctx) & LISTEN_CABLE) {
			post_command(&start_Conveyor, signal, value, ctx);
		}
		return;
	}

	stop_Conveyor(signal, value, ctx);
}

void do_process_local_stop_stars(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	stop_Stars(signal, value, ctx);
}

void process_local_stars(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(control_mode(ctx) & (LISTEN_RPDU | LISTEN_CABLE)) {
		signal->s_value = value;

		int forward = signal_get(ctx, "dev.485.rpdu485.kei.loader_up") && (control_mode(ctx) & LISTEN_RPDU) ||
									signal_get(ctx, "dev.485.rpdu485c.kei.loader_up") && (control_mode(ctx) & LISTEN_CABLE);
		int reverse = signal_get(ctx, "dev.485.rpdu485.kei.loader_down") && (control_mode(ctx) & LISTEN_RPDU) ||
									signal_get(ctx, "dev.485.rpdu485c.kei.loader_down") && (control_mode(ctx) & LISTEN_CABLE);
		int forward_running = context->stars == STARS_FWD;
		int reverse_running = context->stars == STARS_REV;

		printf("Forward: %d; reverse: %d; forward_running: %d; reverse_running: %d\n", forward, reverse, forward_running, reverse_running);
		if((forward && reverse) || (!reverse && !forward)) {
			printf("Stopping stars\n");
			context->stars = 0;
			stop_Stars(signal, value, ctx);
			post_command(&do_process_local_stop_stars, signal, value, ctx);
		} else if(forward && !forward_running) {
			printf("Starting stars forward\n");
			context->stars = STARS_FWD;
			post_command(&start_Stars, signal, 0, ctx);
		} else if(reverse && !reverse_running) {
			printf("Starting stars reverse\n");
			context->stars = STARS_REV;
			post_command(&start_Stars, signal, 1, ctx);
		}
		return;
	}

	if(!value || function_mode(ctx) == MODE_PUMP) return;
	if(!strcmp(signal->s_name, "dev.485.kb.key.start_stars")) {
		if(control_mode(ctx) & LISTEN_LOCAL) {
			post_command(&start_Stars, signal, 0, ctx);
		}
		return;
	}

	stop_Stars(signal, value, ctx);
}

void process_local_stop_loading(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!value || function_mode(ctx) == MODE_PUMP) return;

	stop_Stars(signal, value, ctx);
	stop_Overloading(signal, value, ctx);
	stop_Conveyor(signal, value, ctx);
}

void process_local_oil_station(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	printf("Processing oil station (%d)\n", value);
	if(!value || function_mode(ctx) == MODE_PUMP) return;
	if(!strcmp(signal->s_name, "dev.485.kb.key.start_oil_station")) {
		if(control_mode(ctx) & LISTEN_LOCAL) {
      printf("Posting start oil station\n");
			post_command(&start_Oil, signal, value, ctx);
		}
    else printf("Wrong control mode\n");
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.oil_station_up")) {
		printf("Checking control mode\n");
		if(control_mode(ctx) & LISTEN_RPDU) {
			printf("Posting command\n");
			post_command(&start_Oil, signal, value, ctx);
		}
    else printf("Wrong control mode\n");
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.oil_station_up")) {
		printf("Checking control mode\n");
		if(control_mode(ctx) & LISTEN_CABLE) {
			printf("Posting command\n");
			post_command(&start_Oil, signal, value, ctx);
		}
    else printf("Wrong control mode\n");
		return;
	}

	stop_Oil(signal, value, ctx);
}

void process_local_hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!value || function_mode(ctx) == MODE_PUMP) return;
	if(!strcmp(signal->s_name, "dev.485.kb.key.start_hydratation")) {
		if(control_mode(ctx) & LISTEN_LOCAL) {
			post_command(&start_Hydratation, signal, value, ctx);
		}
		return;
	}

	stop_Hydratation(signal, value, ctx);
}

void process_local_exec(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!value || function_mode(ctx) == MODE_PUMP) return;
	if(!strcmp(signal->s_name, "dev.485.kb.key.start_exec_dev")) {
		if(control_mode(ctx) & LISTEN_LOCAL) {
			post_command(&start_Hydratation, signal, value, ctx);
			post_command(&start_Organ, signal, value, ctx);
		}
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.exec_dev_up")) {
		if(control_mode(ctx) & LISTEN_RPDU) {
			post_command(&start_Hydratation, signal, value, ctx);
			post_command(&start_Organ, signal, value, ctx);
		}
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.exec_dev_up")) {
		if(control_mode(ctx) & LISTEN_CABLE) {
			post_command(&start_Hydratation, signal, value, ctx);
			post_command(&start_Organ, signal, value, ctx);
		}
		return;
	}

	stop_Organ(signal, value, ctx);
	stop_Hydratation(signal, value, ctx);
}

void process_local_pumping(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!value || function_mode(ctx) != MODE_PUMP) return;
	if(!strcmp(signal->s_name, "dev.485.kb.key.start_oil_pump")) {
		if(control_mode(ctx) & LISTEN_LOCAL) {
			post_command(&start_Pumping, signal, value, ctx);
		}
		return;
	}

	stop_Pumping(signal, value, ctx);
}

void process_local_check(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
}

void do_start_all(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!context->in_progress[ALL]) {
		return;
	}
	int step = value;
	switch(step) {
	case 0:
		post_command(&do_start_all, signal, step + 1, ctx);
		start_Oil(signal, value, ctx);
		break;
	case 1:
		post_command(&do_start_all, signal, step + 1, ctx);
		start_Overloading(signal, value, ctx);
		break;
	case 2:
		post_command(&do_start_all, signal, step + 1, ctx);
		start_Conveyor(signal, value, ctx);
		break;
	case 3:
		post_command(&do_start_all, signal, step + 1, ctx);
		start_Stars(signal, 0, ctx);
		break;
	case 4:
		post_command(&do_start_all, signal, step + 1, ctx);
		start_Hydratation(signal, value, ctx);
		break;
	case 5:
		post_command(&do_start_all, signal, step + 1, ctx);
		start_Organ(signal, value, ctx);
		break;
	default:
		printf("Full start completed!\n");
		break;
	}
}

void do_stop_all(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	stop_all(signal, value, ctx);
}

void process_local_all(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!value || function_mode(ctx) == MODE_PUMP) return;
	if(!strcmp(signal->s_name, "dev.485.kb.kei1.start_all")) {
		if(control_mode(ctx) & LISTEN_LOCAL) {
			context->in_progress[ALL] = 1;
			post_command(&do_start_all, signal, 0, ctx);
		}
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485.kei.start_all")) {
		if(control_mode(ctx) & LISTEN_RPDU) {
			context->in_progress[ALL] = 1;
			post_command(&do_start_all, signal, 0, ctx);
		}
		return;
	} else if(!strcmp(signal->s_name, "dev.485.rpdu485c.kei.start_all")) {
		if(control_mode(ctx) & LISTEN_CABLE) {
			context->in_progress[ALL] = 1;
			post_command(&do_start_all, signal, 0, ctx);
		}
		return;
	}

	context->in_progress[ALL] = 0;
	stop_all(signal, value, ctx);
}

static void process_mode_switch(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(signal) signal->s_value = value;
	int mode1 = signal_get(ctx, "dev.485.kb.kei1.mode1");
	int mode2 = signal_get(ctx, "dev.485.kb.kei1.mode2");
	int control1 = signal_get(ctx, "dev.485.kb.kei1.control1");
	int control2 = signal_get(ctx, "dev.485.kb.kei1.control2");
	int post_conv = signal_get(ctx, "dev.485.kb.kei1.post_conveyor");
	int state = control1 | (control2 << 1) | (mode1 << 2) | (mode2 << 3);

	if((state & MODE_MASK) != context->function_mode) {
		if(signal) stop_all(signal, value, ctx);
		if(signal) stop_Pumping(signal, value, ctx);
		if(signal) stop_Hydraulics(signal, value, ctx);
		context->function_mode = state & MODE_MASK;
	}

	if((state & MODE_MASK) == MODE_DIAG) {
		context->diagnostic = 1;
	} else {
		context->diagnostic = 0;
	}

	switch(state & CONTROL_MASK) {
	case CONTROL_RADIO:
		context->control_mode = LISTEN_RPDU;
    post_update_command(ctx, "dev.panel10.system_radio", 1);
    post_update_command(ctx, "dev.panel10.system_mestno", 0);
    post_update_command(ctx, "dev.panel10.system_mode", 3);
		break;
	case CONTROL_MANU:
		context->control_mode = LISTEN_LOCAL;
    post_update_command(ctx, "dev.panel10.system_radio", 0);
    post_update_command(ctx, "dev.panel10.system_mestno", 1);
    post_update_command(ctx, "dev.panel10.system_mode", 1);
		break;
	case CONTROL_CABLE:
		context->control_mode = LISTEN_CABLE;
    post_update_command(ctx, "dev.panel10.system_radio", 0);
    post_update_command(ctx, "dev.panel10.system_mestno", 0);
    post_update_command(ctx, "dev.panel10.system_mode", 2);
		break;
	}

	printf("State: %d\n", state & CONTROL_MASK);
	post_update_command(ctx, "dev.panel10.system_pultk", post_conv);

	if(post_conv) {
		context->control_mode |= LISTEN_CONV;
	}
	printf("Control mode: %d\n", context->control_mode);
	printf("Conveyor post mode: %d\n", post_conv);
}
