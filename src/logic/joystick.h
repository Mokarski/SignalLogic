#pragma once
#include <client/client.h>

void process_joystick_register(struct execution_context_s *ctx);
void process_joystick_conv(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_joystick_move(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_joystick_execdev(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_joystick_telescope(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_joystick_support(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_joystick_feeder(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_mode_switch(struct signal_s *signal, int value, struct execution_context_s *ctx);
