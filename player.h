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

    double last_x;
    double last_y;
    double last_z;
    
} Player;

void Player_disconnect(Player *player, char *reason);
void Player_free(Player *player);

void Player_break_block(Player *player, Server *s,  int x, int y, int z);
void Player_place_block(Player *player, Server *s, int x, int y, int z);

void Player_send_keep_alive(Player *player);
void Player_send_message(Player *player, char *msg);
void Player_set_position(Player *player, double x, double y, double z);
void Player_send_new_player(Player *player, Player *newplayer);

void Player_send_player_position(Player *player, Player *other);
void Player_tick_lastlocation(Player *player);
#endif
