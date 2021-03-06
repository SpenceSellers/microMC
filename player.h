#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>
#include "server.h"
#include "map.h"
#include "inventory.h"
typedef struct Server Server; // Cyclic Redundancy fix
typedef struct Player {
    pthread_rwlock_t lock;
    
    char *username;
    int entity_id;
    int socket;
    
    Inventory *inventory;
    int held_slot_num;
    
    double x;
    double y;
    double z;
    float yaw;
    float pitch;
    
    double last_x;
    double last_y;
    double last_z;
    
} Player;
//Player *Player_new_default(char *username, int socket);
void Player_free(Player *player);

void Player_disconnect(Player *player, char *reason);

/* Player_break_block and Player_place_block will probably
 * the server's map_lock will probably need to be locked for writing,
 * Along with the player's own lock, as usual.
 */
void Player_break_block(Player *player, Server *s,  int x, int y, int z);
void Player_place_block(Player *player, Server *s, int x, int y, int z);

/* This is used to change the player's position while they are
 * moving normally. Changing this will NOT send the player's new
 * position to the client. This function should only be used when
 * the client already knows where they are.*/
void Player_set_position(Player *player, double x, double y, double z);
void Player_set_look(Player *player, float yaw, float pitch);
/* This is used to move the player to a new location for reasons which
 * the client DOESN'T expect. This will send the new location to the client.
 */
void Player_teleport(Player *player, double x, double y, double z);

void Player_send_keep_alive(Player *player);
void Player_send_block_change(Player *player, Block b, int x, int y, int z);
void Player_send_message(Player *player, char *msg);
void Player_send_new_player(Player *player, Player *newplayer);
void Player_send_inventory_update(Player *player);
void Player_send_pos_and_look_update(Player *player);
void Player_send_player_position(Player *player, Player *other);
void Player_send_slot(Player *player, Slot *slot, short slot_id);

void Player_tick_lastlocation(Player *player);

void Player_set_slot(Player *player, Slot *slot, short slot_id);
Slot *Player_get_held_slot(Player *player);
void Player_give_slot(Player *player, Slot *slot);
#endif
