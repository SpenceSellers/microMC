#include "playerconnectionhandlers.h"
#include "logging.h"
#include "inventory.h"
#include "commands.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFERSIZE 2048
Player * handle_login(int sock, Server *s){
    logmsg(LOG_DEBUG, "Handling a login...");
    size_t sent;
    char buffer[BUFFERSIZE];
    int read = recv(sock, buffer, BUFFERSIZE, 0);
    if (read <= 0){
	logmsg(LOG_INFO, "Socket has been closed!");
	close(sock);
	pthread_exit(NULL);
    }
    if (buffer[0] != PACKET_HANDSHAKE){
	logmsg(LOG_DEBUG, "Someone connected, but is not a minecraft login!");
	return NULL;
    }
    
    Packet02Handshake *handshake = Packet02Handshake_parse(buffer, read);

    /* Copy username over to the Player struct.
     * The packet struct's username string will be freed, so
     * the pointer can't just be transferred.
     */
    size_t username_length = strlen(handshake->username) + 1;
    char *playername = malloc(sizeof(char) * username_length);
    memcpy(playername, handshake->username, username_length);
    
    Packet02Handshake_free(handshake);

    Player *player = malloc(sizeof(Player));
    pthread_rwlock_init(&player->lock, NULL);
    player->username = playername;
    player->socket = sock;
    player->x = (double) s->spawnx;
    player->y = (double) s->spawny;
    player->z = (double) s->spawnz;
    player->yaw = 0;
    player->pitch = 0;
    player->inventory = Inventory_new_empty(45);
    player->held_slot_num = 0;
    
    /*
     * Login Request
     */

    Packet01LoginRequest *loginreq = malloc(sizeof(Packet01LoginRequest));
    loginreq->entity_id = 10;
    loginreq->level_type = strdup("default");
    loginreq->game_mode = 0; //0 is survival, 1 is creative.
    loginreq->difficulty = 2;
    loginreq->dimension = 0;
    loginreq->max_players = 50;

    size_t packlen;
    char *data = Packet01LoginRequest_encode(loginreq, &packlen);
        
    Packet01LoginRequest_free(loginreq);
    logmsg(LOG_DEBUG, "Sending login request.");
    sent = send(sock, data, packlen, 0);
    free(data);

    if (sent != packlen) logmsg(LOG_ERROR, "NOT ENOUGH DATA SENT");

    Packet06SpawnPosition *spawnpos = malloc(sizeof(Packet06SpawnPosition));
    spawnpos->x = s->spawnx;
    spawnpos->y = s->spawny;
    spawnpos->z = s->spawnz;

    char *data2;
    data2 = Packet06SpawnPosition_encode(spawnpos, &packlen);
    Packet06SpawnPosition_free(spawnpos);
    logmsg(LOG_DEBUG, "Sending Spawn Position");
    sent = send(sock, data2, packlen, 0);
    debug_print_hex_string(data2, packlen);
    free(data2);
    if (sent != packlen) logmsg(LOG_ERROR, "NOT ENOUGH DATA SENT");

    /*
     *Position and Look
     */
    
    Packet0DPlayerPositionAndLook *pos_and_look =
	malloc(sizeof(Packet0DPlayerPositionAndLook));

    pos_and_look->x = 0.0;
    pos_and_look->y_stance = 65.0; // Stance in this case.
    pos_and_look->stance_y = 64.0; // Y in this case.
    pos_and_look->z = 0.0;
    pos_and_look->yaw = 0.0;
    pos_and_look->pitch = 0.0;
    pos_and_look->on_ground = 1;

    data = Packet0DPlayerPositionAndLook_encode(pos_and_look, &packlen);
    free(pos_and_look);
    
    sent = send(sock, data, packlen, 0);
    free(data);
    if (sent != packlen) logmsg(LOG_ERROR, "NOT ENOUGH DATA SENT");	
    
    logmsg(LOG_DEBUG, "Done handling login!");
    return player;
    
}

void handle_player_digging(Packet0EPlayerDigging *packet, Player *p, Server *s){
    printf("%s just broke a block.\n", p->username);
    if (packet->action == 2){ // Finished Digging
	pthread_rwlock_wrlock(&s->map_lock);
	pthread_rwlock_wrlock(&s->players_lock);
	pthread_rwlock_wrlock(&p->lock);
	Player_break_block(p, s, packet->x, packet->y, packet->z);
	pthread_rwlock_unlock(&p->lock);
	pthread_rwlock_unlock(&s->map_lock);
	pthread_rwlock_unlock(&s->players_lock);
    } else {
        //printf("action = %d \n", packet->action); 
    }
    
}

void send_all_chunks(Player *p, Map *map){
    logmsg(LOG_INFO, "Sending all chunks to a player!");
    int i;
    int j;
    for (i=0; i < map->xchunks; i++){
        for (j=0; j < map->zchunks; j++){
	    Chunk *c = Map_get_chunk(map, i, j);
	    if (c == NULL) continue;
	    size_t len;
	    char *data = Packet33ChunkData_construct(c, i, j, &len);
	    send(p->socket, data, len, 0);
	    free(data);
	}
    }
}

void send_all_players(Player *player, Server *server){
    pthread_rwlock_rdlock(&server->players_lock);
    int i;
    for (i=0; i < server->num_players; i++){
	if (player == server->players[i]) continue;
	Player_send_new_player(player, server->players[i]);
    }
    pthread_rwlock_unlock(&server->players_lock);
}
	

void handle_player_chat(Packet03ChatMessage *packet, Player *p, Server *s){
    if (packet->str[0] == '/'){
	handle_command(p, s, packet->str);
	return;
    }
    pthread_rwlock_rdlock(&s->players_lock);
    Server_tell_all(s, packet->str);
    pthread_rwlock_unlock(&s->players_lock);
   
}

void handle_player_position(Packet0BPlayerPosition *packet, Player *p, Server *s){
    pthread_rwlock_wrlock(&p->lock);
    Player_set_position(p, packet->x, packet->y, packet->z);
    pthread_rwlock_unlock(&p->lock);
}

void handle_block_placement(Packet0FPlayerBlockPlacement *packet,
			    Player *p, Server *s){
    logmsg(LOG_DEBUG, "Player is placing block!");
    int x = packet->x;
    int y = packet->y;
    int z = packet->z;

    apply_face(packet->direction, &x, &y, &z);
    pthread_rwlock_wrlock(&p->lock);
    pthread_rwlock_wrlock(&s->map_lock);
    Player_place_block(p, s, x,y,z);
    pthread_rwlock_unlock(&s->map_lock);
    pthread_rwlock_unlock(&p->lock);
}

void handle_item_change(Packet10HeldItemChange *packet, Player *p, Server *s){
    logmsg(LOG_DEBUG, "Player is changing slot.");
    pthread_rwlock_wrlock(&p->lock);
    p->held_slot_num = packet->slot_id;
    pthread_rwlock_unlock(&p->lock);
}
