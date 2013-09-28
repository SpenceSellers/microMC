#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>
#include "server.h"

typedef struct Server Server; // Cyclic Redundancy fix
typedef struct Player {
    char *username;
    int entity_id;
    int socket;

    double x;
    double y;
    double z;
    
} Player;

void Player_disconnect(Player *player, char *reason);
void Player_free(Player *player);

void Player_break_block(Player *player, Server *s,  int x, int y, int z);

void Player_send_keep_alive(Player *player);
void Player_send_message(Player *player, char *msg);
void Player_set_position(Player *player, double x, double y, double z);
#endif
