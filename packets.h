#ifndef PACKETS_H
#define PACKETS_H

#include <stdlib.h>
#include "player.h"

#define PACKET_HANDSHAKE 0x02
char * decode_MCString(char *mcstring, size_t *read);

/* Encodes an ASCII string into Minecraft's UTF-16BE format.
   It has no unicode support at this time.
 */
char * encode_MCString(char *string);

Player * packet_handle02handshake(int socket, char *packet, size_t length);


#endif
