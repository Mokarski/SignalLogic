#include "process.h"
#include "keyboard.h"
#include <time.h>
#include <stdio.h>
#include <logic/logic_client.h>
#include <inttypes.h>

#define CHECK(what) 	if(!context->in_progress[what]) return;

int waitForFeedback(struct execution_context_s *ctx, char *name, int timeout, volatile int *what) {
	int oc  = 0;       
	struct timespec start, now;
	clock_gettime(CLOCK_REALTIME, &start);

	while(!oc && (*what)) {
		int exState;
		oc  = signal_get(ctx, name);
		if(oc) continue;
		usleep(10000);
		post_process(g_Ctx);
		clock_gettime(CLOCK_REALTIME, &now);
		if((now.tv_sec > start.tv_sec + timeout) || (now.tv_sec == start.tv_sec + timeout) && (now.tv_nsec >= start.tv_nsec)) {
			printf("Feedback waiting timeout\n");
			return oc;
		}
	}

	return oc;
}

void process_gauge_register(struct execution_context_s *ctx) {
	processor_add(ctx, "dev.485.ad2.adc1_phys_value", &Pressure_Show);
	processor_add(ctx, "dev.485.ad2.adc2_phys_value", &Pressure_Show);
	processor_add(ctx, "dev.485.ad2.adc3_phys_value", &Pressure_Show);
	processor_add(ctx, "dev.485.ad2.adc4_phys_value", &Pressure_Show);
	processor_add(ctx, "dev.485.ad3.adc1_phys_value", &Pressure_Show);

	processor_add(ctx, "dev.485.ad1.adc1_phys_value", &Oil_Show);
	processor_add(ctx, "dev.485.ad1.adc2_phys_value", &Oil_Show);

	processor_add(ctx, "dev.485.ad1.adc3_phys_value", &Water_Show);
	processor_add(ctx, "dev.485.ad1.adc4_phys_value", &Water_Show);

	processor_add(ctx, "dev.wago.oc_mui2.current_m1a", &Exec_Dev_Show);
	processor_add(ctx, "dev.wago.oc_mui2.current_m1b", &Exec_Dev_Show);
	processor_add(ctx, "dev.wago.oc_mui2.current_m1c", &Exec_Dev_Show);

	processor_add(ctx, "dev.485.ad3.adc3_phys_value", &Metan_Show);
	processor_add(ctx, "dev.wago.oc_mui1.Uin_PhaseA", &Voltage_Show);

  Pressure_Show(NULL, 0, ctx);
  Oil_Show(NULL, 0, ctx);
  Water_Show(NULL, 0, ctx);
  Voltage_Show(NULL, 0, ctx);
  Exec_Dev_Show(NULL, 0, ctx);
}

void Pressure_Show(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  if(signal) signal->s_value = value;

	int H1 = adc_to_hr(ctx, "dev.conf.logic.oil.pressure1", signal_get(ctx, "dev.485.ad2.adc1_phys_value"));
	int H2 = adc_to_hr(ctx, "dev.conf.logic.oil.pressure2", signal_get(ctx, "dev.485.ad2.adc2_phys_value"));
	int H3 = adc_to_hr(ctx, "dev.conf.logic.oil.pressure3", signal_get(ctx, "dev.485.ad2.adc3_phys_value"));
	int H4 = adc_to_hr(ctx, "dev.conf.logic.oil.pressure4", signal_get(ctx, "dev.485.ad2.adc4_phys_value"));
	int H5 = adc_to_hr(ctx, "dev.conf.logic.oil.pressure5", signal_get(ctx, "dev.485.ad3.adc1_phys_value"));

	post_update_command(ctx, "dev.panel10.system_pressure1", H1);
	post_update_command(ctx, "dev.panel10.system_pressure2", H2);
	post_update_command(ctx, "dev.panel10.system_pressure3", H3);
	post_update_command(ctx, "dev.panel10.system_pressure4", H4);
	post_update_command(ctx, "dev.panel10.system_pressure5", H5);
	post_process(ctx);
}

void Oil_Show(struct signal_s *signal, int value, struct execution_context_s *ctx){
  if(signal)
    signal->s_value = value;

	int Oil_level = adc_to_hr(ctx, "dev.conf.logic.oil.level", signal_get(ctx, "dev.485.ad1.adc1_phys_value"));
	int Oil_temp  = adc_to_hr(ctx, "dev.conf.logic.oil.temp", signal_get(ctx, "dev.485.ad1.adc2_phys_value"));

	post_update_command(ctx, "dev.panel10.system_oil_level", Oil_level);
	post_update_command(ctx, "dev.panel10.system_oil_temp", Oil_temp);
}

void Water_Show(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  if(signal) signal->s_value = value;
	int water_flow = adc_to_hr(ctx, "dev.conf.logic.water.flow", signal_get(ctx, "dev.485.ad1.adc3_phys_value"));
	int water_pressure = adc_to_hr(ctx, "dev.conf.logic.water.pressure", signal_get(ctx, "dev.485.ad1.adc4_phys_value"));
	post_update_command(ctx, "dev.panel10.system_water_flow",water_flow);
	post_update_command(ctx, "dev.panel10.system_water_pressure",water_pressure);
}


void Exec_Dev_Show(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  if(signal) signal->s_value = value;
	int m1_Ia = signal_get(ctx, "dev.wago.oc_mui2.current_m1a");
	int m1_Ib = signal_get(ctx, "dev.wago.oc_mui2.current_m1b");
	int m1_Ic = signal_get(ctx, "dev.wago.oc_mui2.current_m1c");
	int I_all=0;

//	int Volt = Get_Signal("dev.wago.oc_mui1.Uin_PhaseA");

  if ((m1_Ia+m1_Ib+m1_Ic) > 0){
      I_all=(m1_Ia+m1_Ib+m1_Ic)/3;
	   }
	int P_m1=(I_all*100)/150;	
	int rot=35;
	post_update_command(ctx, "dev.panel10.system_execdev_load", P_m1);
	post_update_command(ctx, "dev.panel10.system_execdev_rotation",rot);
}

void Metan_Show(struct signal_s *signal, int value, struct execution_context_s *ctx) {

	//READ_SIGNAL("485.ad3.adc3_phys_value");
  if(signal) signal->s_value = value;
	int Metan = Get_Signal("dev.485.ad3.adc3_phys_value");
	if (Metan > 0) Metan =1;
	post_update_command(ctx, "dev.panel10.system_metan",Metan);
}

void System_Mode(int n) {
	WRITE_SIGNAL("dev.panel10.system_mode",n);
}
void System_Radio() {
      //READ_SIGNAL("485.rpdu485.connect");
	int radio_connect = Get_Signal("dev.485.rpdu485.connect");
	WRITE_SIGNAL("dev.panel10.system_radio",radio_connect);
 }

void Pultk_Mode(){
	//READ_SIGNAL("485.kb.kei1.post_conveyor");
	int puk = Get_Signal("dev.485.kb.kei1.post_conveyor");
	WRITE_SIGNAL("dev.panel10.system_pultk",puk);
}
void Voltage_Show(struct signal_s *signal, int value, struct execution_context_s *ctx) {
  if(signal) signal->s_value = value;
	int Volt = Get_Signal("dev.wago.oc_mui1.Uin_PhaseA");
	post_update_command(ctx, "dev.panel10.system_voltage",Volt);
}

void start_Overloading(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(context->in_progress[OVERLOADING]) return;
	context->in_progress[OVERLOADING] = STARTING;
	printf("Starting overloading\n");

	control_Overloading(signal, value, ctx); // Check temp
	CHECK(OVERLOADING);

	post_write_command(ctx, "dev.485.kb.kbl.led_contrast", 50);
	post_write_command(ctx, "dev.485.kb.kbl.start_reloader", 1);
	post_write_command(ctx, "dev.485.rpdu485.kbl.reloader_green", 1);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.reloader_green", 1);

	post_update_command(ctx, "dev.panel10.system_state_code",20);
	process_sirens_timeout(5, &context->in_progress[OVERLOADING], ctx);
	CHECK(OVERLOADING);

	int bki = signal_get(ctx, "dev.wago.bki_k6.M6");
	if(bki) {
		printf("BKI error!\n");
		stop_Overloading(signal, value, ctx);
	}
	control_Overloading(signal, value, ctx); // Check temp
	CHECK(OVERLOADING);

	post_write_command(ctx, "dev.wago.oc_mdo1.ka5_1", 1);
//	post_write_command(ctx, "dev.panel10.system_state_code",20);
	post_update_command(ctx, "dev.panel10.kb.key.reloader",1);
	if(!waitForFeedback(ctx, "dev.wago.oc_mdi1.oc_w_k5", 3, &context->in_progress[OVERLOADING])) {
    control_Overloading(signal, value, ctx); // Check temp
		return;
	}

	context->in_progress[OVERLOADING] = RUNNING;
	control_Overloading(signal, value, ctx);
}

void start_Conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(context->in_progress[CONVEYOR]) return;
	printf("Starting conveyor\n");
	context->in_progress[CONVEYOR] = STARTING;

	control_Conveyor(signal, value, ctx); // Check temp
	CHECK(CONVEYOR);

	post_write_command(ctx, "dev.485.kb.kbl.led_contrast", 50);
	post_write_command(ctx, "dev.485.kb.kbl.start_conveyor", 1);
	post_write_command(ctx, "dev.485.rpdu485.kbl.conveyor_green", 1);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.conveyor_green", 1);

	post_update_command(ctx, "dev.panel10.system_state_code",16);
	process_sirens_timeout(5, &context->in_progress[CONVEYOR], ctx);
	CHECK(CONVEYOR);

	int bki = signal_get(ctx, "dev.wago.bki_k3_k4.M3_M4");
	if(bki) {
		printf("BKI error!\n");
		stop_Conveyor(signal, value, ctx);
	}
	CHECK(CONVEYOR);

	post_write_command(ctx, "dev.wago.oc_mdo1.ka3_1", 1);
	post_update_command(ctx, "dev.panel10.system_state_code",2);
	post_update_command(ctx, "dev.panel10.kb.key.conveyor",1);
	if(!waitForFeedback(ctx, "dev.wago.oc_mdi1.oc_w_k3", 3, &context->in_progress[CONVEYOR])) {
    control_Conveyor(signal, value, ctx); // Check temp
		return;
	}

	context->in_progress[CONVEYOR] = RUNNING;
	control_Conveyor(signal, value, ctx);
}

void start_Stars(struct signal_s *signal, int reverse, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(context->in_progress[STARS]) return;
	context->in_progress[STARS] = STARTING;
	control_Stars(signal, reverse, ctx);
	CHECK(STARS);
	printf("Starting stars%s\n", reverse ? " reverse" : "");
	post_write_command(ctx, "dev.485.kb.kbl.led_contrast", 50);
	post_write_command(ctx, "dev.485.kb.kbl.start_stars", 1);
	post_write_command(ctx, "dev.485.rpdu485.kbl.loader_green", 1);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.loader_green", 1);

	post_update_command(ctx, "dev.panel10.system_state_code", 40);
	process_sirens_timeout(5, &context->in_progress[STARS], ctx);

	CHECK(STARS);
	if(!reverse) {
		post_write_command(ctx, "dev.485.rsrs.rm_u2_on7", 0);
		post_write_command(ctx, "dev.485.rsrs.rm_u2_on6", 1);
	} else {
		post_write_command(ctx, "dev.485.rsrs.rm_u2_on6", 0);
		post_write_command(ctx, "dev.485.rsrs.rm_u2_on7", 1);
	}
	post_update_command(ctx, "dev.panel10.kb.key.stars",1);
	post_update_command(ctx, "dev.panel10.system_state_code",3);
	context->in_progress[STARS] = RUNNING;
	control_Stars(signal, reverse, ctx);
}

void start_Oil(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(context->in_progress[OIL]) return;
	printf("Starting oil station\n");
	post_write_command(ctx, "dev.485.kb.kbl.led_contrast", 50);
	post_write_command(ctx, "dev.485.kb.kbl.start_oil_station", 1);
	post_write_command(ctx, "dev.485.rpdu485.kbl.oil_station_green", 1);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.oil_station_green", 1);
	context->in_progress[OIL] = STARTING;

	control_Oil(signal, value, ctx); // Check temp
	CHECK(OIL);

	post_update_command(ctx, "dev.panel10.system_state_code", 14);
	process_sirens_timeout(5, &context->in_progress[OIL], ctx);
	CHECK(OIL);

	control_Oil(signal, value, ctx); // Check temp
	CHECK(OIL);

	post_write_command(ctx, "dev.wago.oc_mdo1.ka2_1", 1);
	post_update_command(ctx, "dev.panel10.system_state_code", 4);
	post_update_command(ctx, "dev.panel10.kb.key.oil_station", 1);
	if(!waitForFeedback(ctx, "dev.wago.oc_mdi1.oc_w_k2", 3, &context->in_progress[OIL])) {
    control_Oil(signal, value, ctx); // Check temp
		return;
	}

	context->in_progress[OIL] = RUNNING;
	control_Oil(signal, value, ctx);
}

void start_Hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(context->in_progress[HYDRATATION]) return;
	printf("Starting hydratation\n");
	context->in_progress[HYDRATATION] = STARTING;

	control_Hydratation(signal, value, ctx); // Check temp
	CHECK(HYDRATATION);

	post_write_command(ctx, "dev.485.kb.kbl.led_contrast", 50);
	post_write_command(ctx, "dev.485.kb.kbl.start_hydratation", 1);

	post_update_command(ctx, "dev.panel10.system_state_code",18);
	process_sirens_timeout(5, &context->in_progress[HYDRATATION], ctx);
	CHECK(HYDRATATION);

	int bki = signal_get(ctx, "dev.wago.bki_k5.M5");
	if(bki) {
		printf("BKI error!\n");
		stop_Hydratation(signal, value, ctx);
	}
	control_Hydratation(signal, value, ctx); // Check temp
	CHECK(HYDRATATION);

	post_write_command(ctx, "dev.wago.oc_mdo1.ka4_1", 1);
	post_write_command(ctx, "dev.wago.oc_mdo1.water1", 1);
	post_update_command(ctx, "dev.panel10.system_state_code",5);
	post_update_command(ctx, "dev.panel10.kb.key.hydratation",1);
	if(!waitForFeedback(ctx, "dev.wago.oc_mdi1.oc_w_k4", 3, &context->in_progress[HYDRATATION])) {
    control_Hydratation(signal, value, ctx); // Check temp
		return;
	}

	context->in_progress[HYDRATATION] = RUNNING;
	control_Hydratation(signal, value, ctx);
}

void start_Pumping(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(context->in_progress[PUMPING]) return;
	printf("Starting hydratation\n");
	context->in_progress[PUMPING] = STARTING;

	control_Pumping(signal, value, ctx); // Check temp
	CHECK(PUMPING);

	post_write_command(ctx, "dev.485.kb.kbl.led_contrast", 50);
	post_write_command(ctx, "dev.485.kb.kbl.start_oil_pump", 1);
	post_update_command(ctx, "dev.panel10.system_state_code",18);

	process_sirens_timeout(5, &context->in_progress[PUMPING], ctx);
	CHECK(PUMPING);

	int bki = signal_get(ctx, "dev.wago.bki_k7.M7");
	if(bki) {
		printf("BKI error!\n");
		stop_Pumping(signal, value, ctx);
	}

	control_Pumping(signal, value, ctx); // Check temp
	CHECK(PUMPING);

	post_write_command(ctx, "dev.wago.oc_mdo1.ka6_1", 1);
	if(!waitForFeedback(ctx, "dev.wago.oc_mdi1.oc_w_k6", 3, &context->in_progress[PUMPING])) {
    control_Pumping(signal, value, ctx); // Check temp
		return;
	}

	context->in_progress[PUMPING] = RUNNING;
	control_Pumping(signal, value, ctx);
}

void start_Organ(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(context->in_progress[HYDRATATION] != RUNNING && !context->diagnostic) return;
	if(context->in_progress[ORGAN]) return;
	printf("Starting organ\n");
	context->in_progress[ORGAN] = STARTING;

	control_Organ(signal, value, ctx); // Check temp
	CHECK(ORGAN);

	post_write_command(ctx, "dev.485.kb.kbl.led_contrast", 50);
	post_write_command(ctx, "dev.485.kb.kbl.start_exec_dev", 1);
	post_write_command(ctx, "dev.485.rpdu485.kbl.exec_dev_green", 1);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.exec_dev_green", 1);


	post_update_command(ctx, "dev.panel10.system_state_code",12);
	process_sirens_timeout(5, &context->in_progress[ORGAN], ctx);
	CHECK(ORGAN);

	int bki = signal_get(ctx, "dev.wago.bki_k1.M1");
	if(bki) {
		printf("BKI error!\n");
		stop_Organ(signal, value, ctx);
	}
	control_Organ(signal, value, ctx); // Check temp
	CHECK(ORGAN);

	post_write_command(ctx, "dev.wago.oc_mdo1.ka1_1", 1);
	post_update_command(ctx, "dev.panel10.system_state_code",6);
	post_update_command(ctx, "dev.panel10.kb.key.exec_dev",1);
	if(!waitForFeedback(ctx, "dev.wago.oc_mdi1.oc_w_k1", 3, &context->in_progress[ORGAN])) {
    control_Organ(signal, value, ctx); // Check temp
		return;
	}

	context->in_progress[ORGAN] = RUNNING;
	control_Organ(signal, value, ctx);
}

int control_motor(struct execution_context_s *ctx, const char *prefix, const char *config_prefix, const char *name) {
  char signal_name[64];
  int voltage, max_voltage, min_voltage;
  const char config_660[] = ".660";
  const char config_1140[] = ".1140";
  const char *vprefix = config_660;
  const char *wago_config[] = {
    ".nominal_current", ".start_current", ".start_time",
    ".overload_time",   ".overturn_time", 
    ".thermal_relay", ".temp", NULL
  };
  int i, value;

  voltage = signal_get(ctx, "dev.wago.config.voltage");
  max_voltage = signal_get(ctx, "dev.wago.config.max_voltage");
  min_voltage = signal_get(ctx, "dev.wago.config.min_voltage");
  if(voltage == 0) {
    voltage = signal_get(ctx, "dev.conf.wago.voltage");
    printf("Updating WAGO voltage to %d\n", voltage);
    post_write_command(ctx, "dev.wago.config.voltage", voltage);
  }

  if(voltage > 660) {
    vprefix = config_1140;
  }

  if(max_voltage == 0) {
    max_voltage = signal_get(ctx, "dev.conf.wago.max_voltage");
    printf("Updating WAGO max voltage to %d\n", max_voltage);
    post_write_command(ctx, "dev.wago.config.max_voltage", max_voltage);
  }

  if(min_voltage == 0) {
    min_voltage = signal_get(ctx, "dev.conf.wago.min_voltage");
    printf("Updating WAGO min voltage to %d\n", min_voltage);
    post_write_command(ctx, "dev.wago.config.min_voltage", min_voltage);
  }

  strcpy(signal_name, prefix);
  strcat(signal_name, ".error.all");

  int is_enabled = signal_get(ctx, name);
  int error_map = signal_get(ctx, signal_name);

  //printf("%s: is_enabled: %d; %s: 0x%x\n", prefix, is_enabled, signal_name, error_map);
  if(is_enabled && error_map) {
    printf("%s is enabled and error map is %x\n", prefix, error_map);
    return error_map;
  }

#define NO_CONFIG   1
  if(error_map & NO_CONFIG) {
    printf("Uploading config for %s\n", prefix);
    for(i = 0; wago_config[i] != NULL; i ++) {
      strcpy(signal_name, config_prefix);
      if(i < 5) strcat(signal_name, vprefix); // Thermal settings do not need voltage prefix
      strcat(signal_name, wago_config[i]);
      value = signal_get(ctx, signal_name);
      printf("Reading config %s: %d\n", signal_name, value);

      if(strcmp(wago_config[i], ".thermal_relay") && value == 0) {
        printf("Config %s is 0\n", signal_name);
        return error_map;
      }

      strcpy(signal_name, prefix);
      strcat(signal_name, wago_config[i]);
      printf("Setting %s to %d\n", signal_name, value);
      post_write_command(ctx, signal_name, value);
    }

    strcpy(signal_name, prefix);
    strcat(signal_name, ".ready");
    post_write_command(ctx, signal_name, 0);
    printf("Updating %s\n", signal_name);
    post_process(ctx);

    i = 0;
    printf("Waiting for config processed\n");
    while(signal_get(ctx, signal_name) == 0 && ++ i < 100) {
      usleep(10000);
    }

    if(signal_get(ctx, signal_name) == 0) {
      printf("Config can't be processed\n");
      return error_map;
    }
  }

  strcpy(signal_name, prefix);
  strcat(signal_name, ".error.all");
  i = 0;

  while((signal_get(ctx, signal_name) != 0) && ++ i < 100) {
    usleep(1000);
  }
  error_map = signal_get(ctx, signal_name);
  return error_map;
}

void control_Overloading(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!context->in_progress[OVERLOADING]) return;

  int error = control_motor(ctx, "dev.wago.config.m6", "dev.conf.wago.reloader", "dev.wago.oc_mdo1.ka5_1");
  if(error) {
    post_update_command(ctx, "dev.panel10.motor6.error", error);
    stop_Overloading(signal, value, ctx);
  }
}

void control_Conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!context->in_progress[CONVEYOR]) return;

  int error = control_motor(ctx, "dev.wago.config.m3", "dev.conf.wago.conveyor1", "dev.wago.oc_mdo1.ka3_1");
  if(error) {
    post_update_command(ctx, "dev.panel10.motor3.error", error);
    stop_Conveyor(signal, value, ctx);
  }

  error = control_motor(ctx, "dev.wago.config.m4", "dev.conf.wago.conveyor2", "dev.wago.oc_mdo1.ka3_1");
  if(error) {
    post_update_command(ctx, "dev.panel10.motor4.error", error);
    stop_Conveyor(signal, value, ctx);
  }
}

void control_Stars(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!context->in_progress[STARS]) return;
	if(context->in_progress[OIL] != RUNNING && !context->diagnostic) {
		printf("Oil station is not running!");
		stop_Stars(signal, value, ctx);
	}
}

void control_Oil(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	int enabled = signal_get(ctx, "dev.wago.oc_mdo1.ka2_1", 0);
	if(!context->in_progress[OIL]) return;

  int error = control_motor(ctx, "dev.wago.config.m2", "dev.conf.wago.oil", "dev.wago.oc_mdo1.ka2_1");
  if(error) {
    post_update_command(ctx, "dev.panel10.motor2.error", error);
    stop_Oil(signal, value, ctx);
  }

  int level_state = check_limits(ctx, "dev.conf.logic.oil.level", signal_get(ctx, "dev.485.ad1.adc1_phys_value"));
	if(level_state == LIMIT_MIN_CRIT) {
		printf("Oil station: oil level low!\n");
		stop_Oil(signal, value, ctx);
	}

  int temp_state = check_limits(ctx, "dev.conf.logic.oil.temp", signal_get(ctx, "dev.485.ad1.adc2_phys_value"));
	if(temp_state == LIMIT_MIN_CRIT) {
		printf("Oil station: oil level low!\n");
		stop_Oil(signal, value, ctx);
	}
	if(temp_state == LIMIT_MAX_CRIT) {
		printf("Oil station: oil temp high!\n");
		stop_Oil(signal, value, ctx);
	}

  if(enabled)
  {
    int H1_state = check_limits(ctx, "dev.conf.logic.oil.pressure1", signal_get(ctx, "dev.485.ad2.adc1_phys_value"));
    int H2_state = check_limits(ctx, "dev.conf.logic.oil.pressure2", signal_get(ctx, "dev.485.ad2.adc2_phys_value"));
    int H3_state = check_limits(ctx, "dev.conf.logic.oil.pressure3", signal_get(ctx, "dev.485.ad2.adc3_phys_value"));
    int H4_state = check_limits(ctx, "dev.conf.logic.oil.pressure4", signal_get(ctx, "dev.485.ad2.adc4_phys_value"));
    int H5_state = check_limits(ctx, "dev.conf.logic.oil.pressure5", signal_get(ctx, "dev.485.ad3.adc1_phys_value"));

    if(H1_state == LIMIT_MIN_CRIT) {
      printf("Oil station: oil pressure H1 low!\n");
      stop_Oil(signal, value, ctx);
    }
    if(H1_state == LIMIT_MAX_CRIT) {
      printf("Oil station: oil pressure H1 high!\n");
      stop_Oil(signal, value, ctx);
    }

    if(H2_state == LIMIT_MIN_CRIT) {
      printf("Oil station: oil pressure H2 low!\n");
      stop_Oil(signal, value, ctx);
    }
    if(H2_state == LIMIT_MAX_CRIT) {
      printf("Oil station: oil pressure H2 high!\n");
      stop_Oil(signal, value, ctx);
    }

    if(H3_state == LIMIT_MIN_CRIT) {
      printf("Oil station: oil pressure H3 low!\n");
      stop_Oil(signal, value, ctx);
    }
    if(H3_state == LIMIT_MAX_CRIT) {
      printf("Oil station: oil pressure H3 high!\n");
      stop_Oil(signal, value, ctx);
    }

    if(H4_state == LIMIT_MIN_CRIT) {
      printf("Oil station: oil pressure H4 low!\n");
      stop_Oil(signal, value, ctx);
    }
    if(H4_state == LIMIT_MAX_CRIT) {
      printf("Oil station: oil pressure H4 high!\n");
      stop_Oil(signal, value, ctx);
    }

    if(H5_state == LIMIT_MIN_CRIT) {
      printf("Oil station: oil pressure H5 low!\n");
      stop_Oil(signal, value, ctx);
    }
    if(H5_state == LIMIT_MAX_CRIT) {
      printf("Oil station: oil pressure H5 high!\n");
      stop_Oil(signal, value, ctx);
    }
  }
}

void control_Hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
  int enabled = signal_get(ctx, "dev.wago.oc_mdo1.ka4_1");
	if(!context->in_progress[HYDRATATION]) return;

  int error = control_motor(ctx, "dev.wago.config.m5", "dev.conf.wago.sprinkler", "dev.wago.oc_mdo1.ka4_1");
  if(error) {
    post_update_command(ctx, "dev.panel10.motor5.error", error);
    stop_Hydratation(signal, value, ctx);
  }

  if(enabled)
  {
    int pressure_state = check_limits(ctx, "dev.conf.logic.water.pressure", signal_get(ctx, "dev.485.ad1.adc4_phys_value"));
    int flow_state = check_limits(ctx, "dev.conf.logic.water.flow", signal_get(ctx, "dev.485.ad1.adc3_phys_value"));

    if(pressure_state == LIMIT_MAX_CRIT) {
      printf("sprinklers: water pressure too high!\n");
      stop_Hydratation(signal, value, ctx);
    } else if(pressure_state == LIMIT_MIN_CRIT) {
      printf("sprinklers: water pressure too low!\n");
      stop_Hydratation(signal, value, ctx);
    }

    if(flow_state == LIMIT_MAX_CRIT) {
      printf("sprinklers: water flow rate too high!\n");
      stop_Hydratation(signal, value, ctx);
    } else if(flow_state == LIMIT_MIN_CRIT) {
      printf("sprinklers: water flow rate too low!\n");
      stop_Hydratation(signal, value, ctx);
    }
  }
}

void control_Organ(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!context->in_progress[ORGAN]) return;
	if(context->in_progress[HYDRATATION] != RUNNING && !context->diagnostic) stop_Organ(signal, value, ctx);
	int waterFlow = signal_get(ctx, "dev.485.ad1.adc3.flow");
	//READ_SIGNAL("485.ad1.adc3.flow");

  int error = control_motor(ctx, "dev.wago.config.m1", "dev.conf.wago.drill", "dev.wago.oc_mdo1.ka1_1");
  if(error) {
    post_update_command(ctx, "dev.panel10.motor1.error", error);
    stop_Organ(signal, value, ctx);
  }

	if(waterFlow) {
	}
}

void control_Pumping(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!context->in_progress[PUMPING]) return;
	int oil_level = adc_to_hr(ctx, "dev.conf.logic.oil.level", signal_get(ctx, "dev.485.ad1.adc1_phys_value"));
	int oil_level_max = signal_get(ctx, "dev.conf.logic.oil.level.max");

  int error = control_motor(ctx, "dev.wago.config.m7", "dev.conf.wago.pump", "dev.wago.oc_mdo1.ka6_1");
  if(error) {
    post_update_command(ctx, "dev.panel10.motor7.error", error);
    stop_Pumping(signal, value, ctx);
  }

  int level_state = check_limits(ctx, "dev.conf.logic.oil.level", signal_get(ctx, "dev.485.ad1.adc1_phys_value"));
	if(level_state == LIMIT_MAX_CRIT) {
		printf("Pumping: oil level high!\n");
		stop_Pumping(signal, value, ctx);
	}
}

void stop_Pumping(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	printf("Stopping pumping\n");
	context->in_progress[PUMPING] = 0;
  stop_check_limits(ctx, "dev.conf.logic.oil.level");
	post_write_command(ctx, "dev.485.kb.kbl.start_oil_pump", 0);
	post_write_command(ctx, "dev.wago.oc_mdo1.ka6_1", 0);	
}

void stop_Overloading(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	//if(!context->in_progress[OVERLOADING]) return;
	printf("Stopping overloading\n");
	context->in_progress[OVERLOADING] = 0;
	post_write_command(ctx, "dev.485.rpdu485.kbl.reloader_green", 0);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.reloader_green", 0);
	post_write_command(ctx, "dev.485.kb.kbl.start_reloader", 0);
	post_write_command(ctx, "dev.wago.oc_mdo1.ka5_1", 0);	
	post_update_command(ctx, "dev.panel10.kb.key.reloader",0);
}

void stop_Conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	//if(!context->in_progress[CONVEYOR]) return;
	printf("Stopping conveyor\n");
	context->in_progress[CONVEYOR] = 0;
	post_write_command(ctx, "dev.485.rpdu485.kbl.conveyor_green", 0);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.conveyor_green", 0);
	post_write_command(ctx, "dev.485.kb.kbl.start_conveyor", 0);
	post_write_command(ctx, "dev.wago.oc_mdo1.ka3_1", 0);
	post_update_command(ctx, "dev.panel10.kb.key.conveyor",0);
}

void stop_Stars(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	//if(!context->in_progress[STARS]) return;
	printf("Stopping stars\n");
	context->in_progress[STARS] = 0;
	post_write_command(ctx, "dev.485.rpdu485.kbl.loader_green", 0);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.loader_green", 0);
	post_write_command(ctx, "dev.485.kb.kbl.start_stars", 0);
	post_write_command(ctx, "dev.485.rsrs.rm_u2_on6", 0);
	post_write_command(ctx, "dev.485.rsrs.rm_u2_on7", 0);

	post_update_command(ctx, "dev.panel10.kb.key.stars",0);
}

void stop_Oil(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	if(!context->diagnostic) {
		stop_Stars(signal, value, ctx);
		stop_Hydraulics(signal, value, ctx);
	}
	//if(!context->in_progress[OIL]) return;
	printf("Stopping oil station\n");
	context->in_progress[OIL] = 0;
  stop_check_limits(ctx, "dev.conf.logic.oil.level");
  stop_check_limits(ctx, "dev.conf.logic.oil.temp");
  stop_check_limits(ctx, "dev.conf.logic.oil.pressure1");
  stop_check_limits(ctx, "dev.conf.logic.oil.pressure2");
  stop_check_limits(ctx, "dev.conf.logic.oil.pressure3");
  stop_check_limits(ctx, "dev.conf.logic.oil.pressure4");
  stop_check_limits(ctx, "dev.conf.logic.oil.pressure5");

	post_write_command(ctx, "dev.485.rpdu485.kbl.oil_station_green", 0);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.oil_station_green", 0);
	post_write_command(ctx, "dev.485.kb.kbl.start_oil_station", 0);
	post_write_command(ctx, "dev.wago.oc_mdo1.ka2_1", 0);

	post_update_command(ctx, "dev.panel10.kb.key.oil_station",0);
	printf("Oil station stopped\n");
}

void stop_Hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	context->in_progress[HYDRATATION] = 0;
	if(!context->diagnostic) {
		stop_Organ(signal, value, ctx);
	}
	printf("Stopping hydratation\n");

  stop_check_limits(ctx, "dev.conf.logic.water.flow");
  stop_check_limits(ctx, "dev.conf.logic.water.pressure");

	post_write_command(ctx, "dev.485.kb.kbl.start_hydratation", 0);
	post_write_command(ctx, "dev.wago.oc_mdo1.ka4_1", 0);
	post_write_command(ctx, "dev.wago.oc_mdo1.water1", 0);

	post_update_command(ctx, "dev.panel10.kb.key.hydratation",0);
}

void stop_Organ(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	context->in_progress[ORGAN] = 0;
	printf("Stopping organ\n");
	post_write_command(ctx, "dev.485.rpdu485.kbl.exec_dev_green", 0);
	post_write_command(ctx, "dev.485.rpdu485c.kbl.exec_dev_green", 0);
	post_write_command(ctx, "dev.485.kb.kbl.start_exec_dev", 0);
	post_write_command(ctx, "dev.wago.oc_mdo1.ka1_1", 0);
	post_update_command(ctx, "dev.panel10.kb.key.exec_dev",0);
}

#define	DISABLE(what, value, signal, panel_signal)	\
		if(1) { \
			post_write_command(ctx, signal, 0); \
			if(panel_signal != NULL) post_write_command(ctx, panel_signal, 0); \
		}
void stop_Hydraulics(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	DISABLE(J_LEFT_T, JOYVAL_UP, "dev.485.rsrs.rm_u2_on10", NULL);
	DISABLE(J_LEFT_T, JOYVAL_DOWN, "dev.485.rsrs.rm_u2_on11", NULL);
	DISABLE(J_RIGHT_T, JOYVAL_UP, "dev.485.rsrs.rm_u2_on0", NULL);
	DISABLE(J_RIGHT_T, JOYVAL_DOWN, "dev.485.rsrs.rm_u2_on1", NULL);

	DISABLE(J_ORGAN, JOYVAL_UP, "dev.485.rsrs.rm_u1_on6", NULL);
	DISABLE(J_ORGAN, JOYVAL_DOWN, "dev.485.rsrs.rm_u1_on7", NULL);
	DISABLE(J_ORGAN, JOYVAL_LEFT, "dev.485.rsrs.rm_u1_on3", NULL);
	DISABLE(J_ORGAN, JOYVAL_RIGHT, "dev.485.rsrs.rm_u1_on2", NULL);

	DISABLE(J_ACCEL, JOYVAL_UP, "dev.485.rsrs.rm_u1_on0", NULL);

	DISABLE(J_TELESCOPE, JOYVAL_UP, "dev.485.rsrs.rm_u1_on4", NULL);
	DISABLE(J_TELESCOPE, JOYVAL_DOWN, "dev.485.rsrs.rm_u1_on5", NULL);

	DISABLE(J_SUPPORT, JOYVAL_UP, "dev.485.rsrs.rm_u2_on8", NULL);
	DISABLE(J_SUPPORT, JOYVAL_DOWN, "dev.485.rsrs.rm_u2_on9", NULL);

	DISABLE(J_SOURCER, JOYVAL_UP, "dev.485.rsrs.rm_u1_on8", NULL);
	DISABLE(J_SOURCER, JOYVAL_DOWN, "dev.485.rsrs.rm_u1_on9", NULL);

	DISABLE(J_CONVEYOR, JOYVAL_UP, "dev.485.rsrs.rm_u2_on2", NULL);
	DISABLE(J_CONVEYOR, JOYVAL_DOWN, "dev.485.rsrs.rm_u2_on3", NULL);
	DISABLE(J_CONVEYOR, JOYVAL_LEFT, "dev.485.rsrs.rm_u2_on5", NULL);
	DISABLE(J_CONVEYOR, JOYVAL_RIGHT, "dev.485.rsrs.rm_u2_on4", NULL);
}

void control_all(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
  control_Overloading(signal, value, ctx);
  control_Conveyor(signal, value, ctx);
  control_Stars(signal, value, ctx);
  control_Oil(signal, value, ctx);
  control_Hydratation(signal, value, ctx);
  control_Organ(signal, value, ctx);
  control_Pumping(signal, value, ctx);
}

void stop_all(struct signal_s *signal, int value, struct execution_context_s *ctx) {
	struct logic_context_s *context = (struct logic_context_s*)ctx->clientstate;
	context->in_progress[ALL] = 0;
	stop_Overloading(signal, value, ctx);
	stop_Conveyor(signal, value, ctx);
	stop_Stars(signal, value, ctx);
	stop_Oil(signal, value, ctx);
	stop_Hydratation(signal, value, ctx);
	stop_Organ(signal, value, ctx);
}
