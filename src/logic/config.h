#pragma once
#include <client/client.h>

void process_config_register(struct execution_context_s *ctx);
void process_config_wago(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_config_sound(struct signal_s *signal, int value, struct execution_context_s *ctx);
