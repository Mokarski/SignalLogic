/***************************************************
 * serverevents.c
 * Created on Sun, 22 Oct 2017 18:15:32 +0000 by vladimir
 *
 * $Author$
 * $Rev$
 * $Date$
 ***************************************************/

#include <string.h>
#include <stdlib.h>
#include "common/proto.h"
#include "servercommand.h"
#include "serverevents.h"

int process_events(struct execution_context_s *ctx) {
  struct event_s *event = ctx->events;
  int i, events = 0, slen = 0;
  char *buffer[MAX_CONN] = {0};
  struct subscription_s *subscriptions[MAX_CONN];
  int size;

  while(event) {
    events ++;
    slen += strlen(event->e_signal->s_name);
    event = event->e_next;
  }

  size = slen + events * 20;

  while(event = ctx->events) {
    int found = hash_find_all(ctx->subscription_by_signal, event->e_signal->s_name, (void**)subscriptions, sizeof(subscriptions) / sizeof(void*));

    for(i = 0; i < found; i ++) {
      int socknum = subscriptions[i]->s_socket;

      if(event->e_type != subscriptions[i]->s_type) {
        continue;
      }

      if(buffer[socknum] == NULL) {
        buffer[socknum] = malloc(size);
        cmd_create_packet(buffer[socknum]);
      }

      switch(event->e_type) {
      case SUB_UPDATE:
        cmd_create_command((struct cmd_packet_header_s *)buffer[socknum], size, event->e_signal->s_name, CMD_UPDATE, 1, event->e_value);
        break;
      case SUB_WRITE:
        cmd_create_command((struct cmd_packet_header_s *)buffer[socknum], size, event->e_signal->s_name, CMD_WRITE, 1, event->e_value);
        break;
      }
    }
    ctx->events = event->e_next;
    free(event);
  }

	ctx->events_head = NULL;

  for(i = 0; i < MAX_CONN; i ++) {
    if(buffer[i]) {
      packet_send_command((void*)buffer[i], ctx->sockets[i], ctx, &process_command, NULL);
      free(buffer[i]);
    }
  }
}
