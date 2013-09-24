#include "connection.h"
#include "logging.h"
#include "pthread.h"
#include "player.h"
#include "packets.h"
#include "server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <limits.h>

#define BUFFERSIZE 1024

#define PORT "25565"
#define CONNECTION_BACKLOG 10

Player * handle_login(int sock){
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
     * It the pointer can't just be transferred.
     */
    size_t username_length = strlen(handshake->username) + 1;
    char *playername = malloc(sizeof(char) * username_length);
    memcpy(playername, handshake->username, username_length);
    
    Packet02Handshake_free(handshake);

    Player *player = malloc(sizeof(Player));
    player->username = playername;
    player->socket = sock;
    

    /*
     * Login Request
     */

    Packet01LoginRequest *loginreq = malloc(sizeof(Packet01LoginRequest));
    loginreq->entity_id = 10;
    loginreq->level_type = strdup("default");
    loginreq->game_mode = 0;
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

    Packet06SpawnPosition *spawnpos = malloc(sizeof(spawnpos));
    spawnpos->x = 0;
    spawnpos->y = 64;
    spawnpos->z = 0;

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
struct ConnectionThreadArgs {
    int sock;
    Server *server;
};
    
void *connection_thread(void *args){
    logmsg(LOG_INFO, "Started new connection thread.");
    struct ConnectionThreadArgs *ctargs;
    ctargs = (struct ConnectionThreadArgs *) args;
    
    int sock = ctargs->sock;
    Server *server = ctargs->server;
    
    char buffer[BUFFERSIZE];
    Player *player = handle_login(sock);
    if (player == NULL){
	close(sock);
	pthread_exit(NULL);
    } else {
	pthread_rwlock_wrlock(&server->players_lock);
	Server_add_player(server, player);
	pthread_rwlock_unlock(&server->players_lock);
    }

    while (1){
	ssize_t read = recv(sock, buffer, BUFFERSIZE, 0);
        if (read <= 0){
	    logmsg(LOG_INFO, "Socket has been closed!");
	    close(sock);
	    pthread_exit(NULL);
	    return;
	}
	
    }
}

void *connection_distributor_thread(void *args){
    Server *server = (Server*) args;
    
    logmsg(LOG_INFO, "Started new distributor thread.");

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &res);

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int binderr = bind(sock, res->ai_addr, res->ai_addrlen);
    if (binderr == -1){
	logmsg(LOG_ERROR, "Unable to bind socket!");
	exit(1);
    }
    listen(sock, CONNECTION_BACKLOG);

    addr_size = sizeof their_addr;
    
    server->distributor_socket = sock;
    while (1){
	logmsg(LOG_DEBUG, "Distributor waiting on new connection.");
	new_fd = accept(sock, (struct sockaddr *)&their_addr, &addr_size);
	if (new_fd == -1){
	    logmsg(LOG_ERROR, "Error accepting connection!");
	}
	logmsg(LOG_INFO, "Accepted connection. Starting new thread!");
	struct ConnectionThreadArgs ctargs;
	ctargs.server = server;
	ctargs.sock = new_fd;
	
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&thread, &attr, &connection_thread, &ctargs);
	pthread_attr_destroy(&attr);
    }

    logmsg(LOG_INFO, "Closing distributor socket/thread.");
    close(sock);
    pthread_exit(NULL);
}

    
