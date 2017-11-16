#pragma once
#include <common/signal.h>
#include <client/client.h>

void start_Overloading(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Stars(struct signal_s *signal, int reverse, struct execution_context_s *ctx);
void start_Oil(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Organ(struct signal_s *signal, int value, struct execution_context_s *ctx);
void start_Pumping(struct signal_s *signal, int value, struct execution_context_s *ctx);

void control_Overloading();
void control_Conveyor();
void control_Stars();
void control_Oil();
void control_Hydratation();
void control_Organ();
void control_Pumping();

void stop_Overloading();
void stop_Conveyor();
void stop_Stars();
void stop_Oil();
void stop_Hydratation();
void stop_Organ();
void stop_Pumping();
void stop_Hydraulics();

void start_all(struct signal_s *signal, int value, struct execution_context_s *ctx);
void control_all();
void stop_all();

void set_Diagnostic(int val);
int enabled_Oil();
