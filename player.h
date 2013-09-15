#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>

typedef struct Player {
    int entity_id;
    int socket;
    pthread_t connection_thread;
    
} Player;

typedef struct PlayerList{
    Player *player;
    struct PlayerList *next;
} PlayerList;

void Player_disconnect(Player *player);

#endif
