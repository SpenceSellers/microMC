#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>

typedef struct Player {
    char *username;
    int entity_id;
    int socket;
    pthread_t connection_thread;
    
} Player;
#endif
