#pragma once

#include <common/signal.h>
#include <client/client.h>
#include <client/clientcommand.h>
#include <common/ringbuffer.h>

#define RB_FLUSH() do { post_process(g_Ctx); while(ring_buffer_size(g_Ctx->command_buffer) > 0) pthread_yield(); } while(0);

struct execution_context_s *g_Ctx;

#define WRITE_SIGNAL(signal, value) do { post_write_command(g_Ctx, signal, value); } while(0);
#define READ_SIGNAL(signal)		post_read_command(g_Ctx,signal);

void process_register_common(struct execution_context_s *ctx);
void process_urgent_stop(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_sirenes(struct signal_s *signal, int value, struct execution_context_s *ctx);

int  Get_Signal(char *name);
