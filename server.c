#include "server.h"

#include <stdlib.h>

Server * Server_create(Map *map){
    Server *server = malloc(sizeof(Server));

    server->players = NULL;
    pthread_rwlock_init( &(server->players_lock), NULL);

    server->map = map;

    pthread_rwlock_init( &(server->map_lock), NULL);

    return server;
}
