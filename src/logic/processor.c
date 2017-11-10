#include "processor.h"

void processor_add(struct execution_context_s *ctx, char *name, signal_processor_t processor)
{
	struct logic_context_s *context = ctx->clientstate;
	hash_add(context->proc_hash, name, processor);
}

int processor_do(struct execution_context_s *ctx, struct signal_s *signal, int value)
{
	struct logic_context_s *context = ctx->clientstate;
	signal_processor_t processors[256];
	int i, num = hash_find_all(context->proc_hash, name, (void**)processors, sizeof(processors)/sizeof(processors[0]));
	for(i = 0; i < num; i ++) {
		processors[i](signal, value, ctx);
	}

	return num != 0;
}
