#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>

typedef struct Player {
    char *username;
    int entity_id;
    int socket;
    
} Player;

void Player_disconnect(Player *player);
void Player_free(Player *player);
#endif
