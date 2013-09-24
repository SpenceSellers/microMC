#ifndef SERVER_H
#define SERVER_H

#include "player.h"
#include "map.h"
#include <pthread.h>

typedef struct Server {
    pthread_rwlock_t running_lock;
    char isrunning;
    
    pthread_rwlock_t players_lock;
    PlayerList *players;
    
    pthread_rwlock_t map_lock;
    Map *map;

    /* The thread which spawns new connections.
     * There is no lock because only the main thread
     * should ever need to touch it. */
    pthread_t distributor_thread;

} Server;

Server * Server_create(Map *map);


#endif

