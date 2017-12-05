#pragma once
#include <client/client.h>

#define LISTEN_NONE		0x0
#define LISTEN_LOCAL	0x1
#define LISTEN_RPDU		0x2
#define LISTEN_CONV		0x4
#define LISTEN_CABLE	0x8
#define LISTEN_MAIN		(LISTEN_LOCAL | LISTEN_RPDU | LISTEN_CABLE)

#define MODE_MASK				(0x03 << 2)
#define MODE_DIAG				(0x01 << 2)
#define MODE_PUMP				(0x02 << 2)
#define MODE_NORM				(0x03 << 2)

int control_mode(struct execution_context_s *ctx);
int function_mode(struct execution_context_s *ctx);

void process_local_post_switch_register(struct execution_context_s *ctx);
void process_local_post_register(struct execution_context_s *ctx);
void process_local_reloader(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_conveyor(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_stars(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_oil_station(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_hydratation(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_exec(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_pumping(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_check(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_all(struct signal_s *signal, int value, struct execution_context_s *ctx);
void process_local_stop_loading(struct signal_s *signal, int value, struct execution_context_s *ctx);

