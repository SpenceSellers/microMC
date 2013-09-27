#ifndef PLAYERCONNECTIONHANDLERS_H
#define PLAYERCONNECTIONHANDLERS_H

#include "server.h"
#include "player.h"
#include "packets.h"

Player * handle_login(int sock, Server *server);
void handle_player_chat(Packet03ChatMessage *packet, Player *p, Server *s);
void send_all_chunks(Player *p, Map *map);
void handle_player_digging(Packet0EPlayerDigging *packet, Player *p, Server *s);

#endif
