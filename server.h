#ifndef SERVER_H
#define SERVER_H

#include "player.h"
#include "map.h"
#include <pthread.h>
#include <stdlib.h>

typedef struct Server {
    pthread_rwlock_t running_lock;
    char isrunning;

    size_t max_players;
    
    pthread_rwlock_t players_lock;
    int num_players;
    Player **players;
    
    pthread_rwlock_t map_lock;
    Map *map;

    /* The thread which spawns new connections.
     * There is no lock because only the main thread
     * should ever need to touch it. */
    pthread_t distributor_thread;
    int distributor_socket;

} Server;

Server * Server_create(Map *map, size_t max_players);

/* Returns 1 if the server is full.
 * Else returns 0.
 *
 * players_lock should probably be read locked.
 */
char Server_is_full(Server *server);

/* Adds a player to the server.
 * 
 * This function will probably require the players_lock mutex
 * to be locked for writing.
 */
void Server_add_player(Server *server, Player *player);

/* Removes a player from the player list.
 *  This does NOT free the player object.
 *
 *  players_lock should be write-locked before use.
 */
void Server_remove_player(Server *server, Player *player);

void Server_shutdown(Server *server);
#endif

