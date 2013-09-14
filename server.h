#ifndef SERVER_H
#define SERVER_H

#include "player.h"
#include "map.h"
#include <pthread.h>

typedef struct Server {
    pthread_rwlock_t players_lock;
    PlayerList *players;
    
    pthread_rwlock_t map_lock;
    Map *map;
} Server;

Server * Server_create(Map *map);

#endif

