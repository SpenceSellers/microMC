#ifndef PACKETS_H
#define PACKETS_H

#include <stdlib.h>
#include "player.h"

#define PACKET_HANDSHAKE 0x02
char * decode_MCString(char *mcstring, size_t *read);
Player * packet_handle02handshake(int socket, char *packet, size_t length);


#endif
