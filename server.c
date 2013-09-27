#include "server.h"
#include "logging.h"
#include <stdlib.h>
    
Server * Server_create(Map *map, size_t max_players){
    Server *server = malloc(sizeof(Server));

    server->is_running = 1;
    pthread_rwlock_init(&(server->state_lock), NULL);
    server->time = 0;

    pthread_rwlock_init( &(server->players_lock), NULL);
    
    server->max_players = max_players;
    server->num_players = 0;
    server->players = malloc(sizeof(Player*) * max_players);
    
    server->map = map;

    pthread_rwlock_init( &(server->map_lock), NULL);

    server->distributor_thread = 0;
    
    return server;
}

char Server_is_full(Server *server){
    if (server->num_players >= server->max_players) return 1;
    else return 0;
}
/* Returns the index of a player.
   If the player is not found, returns -1 */
int get_index_of_player(Server *server, Player *player){
    int i;
    for (i=0; i < server->num_players; i++){
	if (server->players[i] == player){
	    return i;
	}
    }
    return -1;
}

void Server_add_player(Server *server, Player *player){
    logmsg(LOG_INFO, "Adding new player to the server!");
    if (Server_is_full(server)) {
	logmsg(LOG_WARN, "Attempted to add more players than is allowed!");
	return;
    }

    server->players[server->num_players] = player;
    server->num_players++;
}

void Server_remove_player(Server *server, Player *player){
    int index = get_index_of_player(server, player);
    
    // Time to shift all of the other players over by one..
    int i;
    for(i = index; i < server->num_players - 2; i++){
	server->players[i] = server->players[i+1];
    }
    server->num_players -= 1;
}

void Server_tick(Server *server){
    server->time++;

    if (server->time % 20 == 0) { // Send keep alives.
	int i;
	for(i=0; i < server->num_players; i++){
	    Player_send_keep_alive(server->players[i]);
	}
    }
}
void Server_shutdown(Server *server){
    server->is_running = 0;
    close(server->distributor_socket);

    int i;
    for(i=0; i < server->num_players; i++){
	Player_disconnect(server->players[i], "Server shutting down!");
    }
}

void Server_change_block(Server *server, Block b, int x, int y, int z){
    logmsg(LOG_DEBUG, "A block is being changed.");
    Map_set_block(server->map, b, x, y, z);
    int i;
    for (i=0; i < server->num_players; i++){
	Player_send_block_change(server->players[i], b, x, y, z);
    }
}
    
