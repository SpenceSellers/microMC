#include "server.h"
#include "logging.h"
#include <string.h>
#include <stdlib.h>
void debug_list_players(Server *server){
    int i;
    printf("%d players logged in.\n", server->num_players);
    for (i=0; i < server->num_players; i++){
	Player *p = server->players[i];
	printf("Index %d, Player: %s, eid: %d, socket %d \n",
	       i, p->username, p->entity_id, p->socket);
    }
}
Server * Server_create(Map *map, size_t max_players){
    Server *server = malloc(sizeof(Server));

    server->is_running = 1;
    pthread_rwlock_init(&(server->state_lock), NULL);
    server->time = 0;
    server->last_entity_id = 1;
    server->spawnx = 0;
    server->spawny = 64;
    server->spawnz = 0;
    
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
    player->entity_id =  ++server->last_entity_id;
    server->players[server->num_players] = player;
    server->num_players++;

    printf("The entity ID is %d \n", player->entity_id);
    int i;
    for (i=0; i < server->num_players; i++){
	if (server->players[i] == player) continue;
	Player_send_new_player(server->players[i], player);
    }
    char msgbuffer[256];
    msgbuffer[0] = 0;
    strcat(msgbuffer, player->username);
    strcat(msgbuffer, " has joined!");
    Server_tell_all(server, msgbuffer);
}

void Server_remove_player(Server *server, Player *player){
    logmsg(LOG_INFO, "Removing player from server.");
    int index = get_index_of_player(server, player);
    
    // Time to shift all of the other players over by one..
    int i;
    for(i = index; i < server->num_players - 1; i++){
	server->players[i] = server->players[i+1];
    }
    server->num_players -= 1;
    logmsg(LOG_DEBUG, "Done removing player.");

    char msgbuffer[256];
    msgbuffer[0] = 0;
    strcat(msgbuffer, player->username);
    strcat(msgbuffer, " has quit.");
    Server_tell_all(server, msgbuffer);
}

void send_player_positions(Server *server){
    int i;
    int j;
    for (i=0; i < server->num_players; i++){
	for(j=0; j < server->num_players; j++){
	    if (server->players[i] == server->players[j]) continue;

	    Player_send_player_position(server->players[i], server->players[j]);
	}
    }
    
}
void Server_tick(Server *server){
    server->time++;

    if (server->time % 20 == 0) { // Send keep alives.
	int i;
	for(i=0; i < server->num_players; i++){
	    Player_send_keep_alive(server->players[i]);
	}
    }
    send_player_positions(server); // This MUST come before ticking lastlocation.
    int i;
    for(i=0; i < server->num_players; i++){
	Player_tick_lastlocation(server->players[i]);
    }
    //debug_list_players(server);
}
void Server_shutdown(Server *server){
    server->is_running = 0;

    int i;
    for(i=0; i < server->num_players; i++){
	Player_disconnect(server->players[i], "Server shutting down!");
	Player_free(server->players[i]);
    }
    close(server->distributor_socket);
}

void Server_change_block(Server *server, Block b, int x, int y, int z){
    logmsg(LOG_DEBUG, "A block is being changed.");
    Map_set_block(server->map, b, x, y, z);
    int i;
    for (i=0; i < server->num_players; i++){
	Player_send_block_change(server->players[i], b, x, y, z);
    }
}
    
void Server_tell_all(Server *server, char *message){
    int i;
    for (i=0; i < server->num_players; i++){
	Player_send_message(server->players[i], message);
    }
}

	
