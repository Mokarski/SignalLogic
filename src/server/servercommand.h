/***************************************************
 * servercommand.h
 * Created on Sat, 21 Oct 2017 06:37:36 +0000 by vladimir
 *
 * $Author$
 * $Rev$
 * $Date$
 ***************************************************/
#ifndef SERVERCOMMAND_H_
#define SERVERCOMMAND_H_

#include <sys/types.h>
#include "signalrouter.h"
#include "common/proto.h"

void process_command(int socket, struct cmd_packet_header_s *hdr, void *context);
int read_command(struct execution_context_s *context, int socket);

#endif
