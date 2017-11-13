#pragma once
#include <client/client.h>

typedef void (*signal_processor_t)(struct signal_s *signal, int value, struct execution_context_s *ctx);

void processor_add(struct execution_context_s *ctx, char *name, signal_processor_t processor);
int  processor_do(struct execution_context_s *ctx, struct signal_s *signal, int value);
