#pragma once
#include <common/signal.h>
#include <client/client.h>
#include <logic/processor.h>

#define IDLE				0
#define STARTING		1
#define RUNNING			2

#define OVERLOADING	0
#define CONVEYOR		1
#define STARS				2
#define OIL					3
#define HYDRATATION	4
#define ORGAN				5
#define PUMPING			6
#define ALL					7
#define DIAG				8

void process_gauge_register(struct execution_context_s *ctx);
void Pressure_Show(struct signal_s *signal, int value, struct execution_context_s *ctx);
void Oil_Show(struct signal_s *signal, int value, struct execution_context_s *ctx);
void Water_Show(struct signal_s *signal, int value, struct execution_context_s *ctx);
void Exec_Dev_Show(struct signal_s *signal, int value, struct execution_context_s *ctx);
void Metan_Show(struct signal_s *signal, int value, struct execution_context_s *ctx);
void Voltage_Show(struct signal_s *signal, int value, struct execution_context_s *ctx);

void start_Overloading(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Stars(struct signal_s *signal, int reverse, struct execution_context_s *ctx);
void start_Oil(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Organ(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Pumping(struct signal_s *signal, int value, struct execution_context_s *ctx);

int control_motor(struct execution_context_s *ctx, const char *prefix, const char *config_prefix, const char *name);
void control_Overloading(struct signal_s *signal, int value, struct execution_context_s *ctx);
void control_Conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx);
void control_Stars(struct signal_s *signal, int value, struct execution_context_s *ctx);
void control_Oil(struct signal_s *signal, int value, struct execution_context_s *ctx);
void control_Hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx);
void control_Organ(struct signal_s *signal, int value, struct execution_context_s *ctx);
void control_Pumping(struct signal_s *signal, int value, struct execution_context_s *ctx);

void stop_Overloading(struct signal_s *signal, int value, struct execution_context_s *ctx);
void stop_Conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx);
void stop_Stars(struct signal_s *signal, int value, struct execution_context_s *ctx);
void stop_Oil(struct signal_s *signal, int value, struct execution_context_s *ctx);
void stop_Hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx);
void stop_Organ(struct signal_s *signal, int value, struct execution_context_s *ctx);
void stop_Pumping(struct signal_s *signal, int value, struct execution_context_s *ctx);
void stop_Hydraulics(struct signal_s *signal, int value, struct execution_context_s *ctx);

void control_all(struct signal_s *signal, int value, struct execution_context_s *ctx);
void stop_all(struct signal_s *signal, int value, struct execution_context_s *ctx);

void set_Diagnostic(int val);
