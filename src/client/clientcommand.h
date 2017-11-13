/***************************************************
 * clientcommand.h
 * Created on Sat, 21 Oct 2017 05:25:09 +0000 by vladimir
 *
 * $Author$
 * $Rev$
 * $Date$
 ***************************************************/
#ifndef CLIENTCOMMAND_H_
#define CLIENTCOMMAND_H_

#include "common/proto.h"
#include "common/hash.h"
#include "common/signal.h"
#include "client.h"

void subscribe(struct execution_context_s *ctx, char *mask, int type);
void unsubscribe(struct execution_context_s *ctx, char *mask, int type);
void write_signal(struct execution_context_s *ctx, char *mask, int value);
void update_signal(struct execution_context_s *ctx, char *mask, int type);
void get_signals(struct execution_context_s *ctx, char *mask);
void get_and_subscribe(struct execution_context_s *ctx, char *mask, int type);

void process_command_write(struct signal_s *signal, struct hash_s *hash, struct cmd_entry_s *command, struct execution_context_s *ctx);
void process_command_update(struct signal_s *signal, struct hash_s *hash, struct cmd_entry_s *command, struct execution_context_s *ctx);
void process_command(int socket, struct cmd_packet_header_s *hdr, void *context);
void parse_response(int socket, struct response_packet_header_s *, void *ctx);

#define read_command(ctx, name) get_signals(ctx, name)
#define write_command(ctx, name, value) write_signal(ctx, name, value)
#define update_command(ctx, name, value) update_signal(ctx, name, value)
#define subscribe_command(ctx, name, type) subscribe(ctx, name, type)
#define unsubscribe_command(ctx, name, type) unsubscribe(ctx, name, type)

void post_process(struct execution_context_s *ctx);
void post_read_command(struct execution_context_s *ctx, char *name);
void post_write_command(struct execution_context_s *ctx, char *name, int value);
void post_update_command(struct execution_context_s *ctx, char *name, int value);
void post_subscribe_command(struct execution_context_s *ctx, char *name, int type);
void post_unsubscribe_command(struct execution_context_s *ctx, char *name, int value);

#endif
