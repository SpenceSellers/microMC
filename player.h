#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>
#include "server.h"
#include "map.h"
#include "inventory.h"
typedef struct Server Server; // Cyclic Redundancy fix
typedef struct Player {
    char *username;
    int entity_id;
    int socket;
    
    Inventory *inventory;
    int held_slot_num;
    
    double x;
    double y;
    double z;

    double last_x;
    double last_y;
    double last_z;
    
} Player;
//Player *Player_new_default(char *username, int socket);
void Player_free(Player *player);

void Player_disconnect(Player *player, char *reason);

void Player_break_block(Player *player, Server *s,  int x, int y, int z);
void Player_place_block(Player *player, Server *s, int x, int y, int z);

void Player_send_keep_alive(Player *player);
void Player_send_block_change(Player *player, Block b, int x, int y, int z);
void Player_send_message(Player *player, char *msg);
void Player_set_position(Player *player, double x, double y, double z);
void Player_send_new_player(Player *player, Player *newplayer);

void Player_send_player_position(Player *player, Player *other);
void Player_tick_lastlocation(Player *player);

void Player_send_slot(Player *player, Slot *slot, short slot_id);
void Player_set_slot(Player *player, Slot *slot, short slot_id);
Slot *Player_get_held_slot(Player *player);

#endif
