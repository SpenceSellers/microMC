#include "connection.h"
#include "logging.h"
#include "pthread.h"
#include "player.h"
#include "packets.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#define BUFFERSIZE 1024

#define PORT "25565"
#define CONNECTION_BACKLOG 10

Player * handle_login(int sock){
    logmsg(LOG_DEBUG, "Handling a login...");
    char buffer[BUFFERSIZE];
    int read = recv(sock, buffer, BUFFERSIZE, 0);
    if (read <= 0){
	logmsg(LOG_INFO, "Socket has been closed!");
	close(sock);
	pthread_exit(NULL);
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

    /* Login Request */

    Packet01LoginRequest *loginreq = malloc(sizeof(Packet01LoginRequest));
    loginreq->entity_id = 10;
    loginreq->level_type = strdup("default");
    loginreq->game_mode = 0;
    loginreq->difficulty = 2;
    loginreq->max_players = 50;

    size_t packlen;
    char *data = Packet01LoginRequest_encode(loginreq, &packlen);
    Packet01LoginRequest_free(loginreq);
    logmsg(LOG_DEBUG, "Sending login request.");
    send(sock, data, packlen, 0);

    Packet06SpawnPosition *spawnpos = malloc(sizeof(Packet06SpawnPosition));
    spawnpos->x = 0;
    spawnpos->y = 60;
    spawnpos->z = 0;

    data = Packet06SpawnPosition_encode(spawnpos, &packlen);
    Packet06SpawnPosition_free(spawnpos);
    logmsg(LOG_DEBUG, "Sending Spawn Position");
    send(sock, data, packlen, 0);

    /* Position and Look */
    
    Packet0DPlayerPositionAndLook *pos_and_look =
	malloc(sizeof(Packet0DPlayerPositionAndLook));

    pos_and_look->x = 0.0;
    pos_and_look->y_stance = 61.0; // Stance in this case.
    pos_and_look->stance_y = 60.0; // Y in this case.
    pos_and_look->z = 0.0;
    pos_and_look->yaw = 0.0;
    pos_and_look->pitch = 0.0;
    pos_and_look->on_ground = 1;

    data = Packet0DPlayerPositionAndLook_encode(pos_and_look, &packlen);
    free(pos_and_look);
    logmsg(LOG_DEBUG, "Sending player position and look.");
    send(sock, data, packlen, 0);
    

    return player;
    
}
void *connection_thread(void *vsock){
    logmsg(LOG_INFO, "Started new connection thread.");
    int *sock = (int *) vsock;
    char buffer[BUFFERSIZE];
    Player *player = handle_login(*sock);
    if (player == NULL){
	logmsg(LOG_INFO, "Someone connected that wasn't logging in!");
	close(*sock);
	pthread_exit(NULL);
    }

    while (1){
	ssize_t read = recv(*sock, buffer, BUFFERSIZE, 0);
        if (read <= 0){
	    logmsg(LOG_INFO, "Socket has been closed!");
	    close(*sock);
	    pthread_exit(NULL);
	    return;
	}
	debug_print_hex_string(buffer, read);
	printf("=======\n");
    }
}

void *connection_distributor_thread(void *nothing){
    
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
    
    
    while (1){
	logmsg(LOG_DEBUG, "Main Thread waiting on new connection.");
	new_fd = accept(sock, (struct sockaddr *)&their_addr, &addr_size);
	if (new_fd == -1){
	    logmsg(LOG_ERROR, "Error accepting connection!");
	}
	logmsg(LOG_INFO, "Accepted connection. Starting new thread!");
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&thread, &attr, &connection_thread, &new_fd);
	pthread_attr_destroy(&attr);
	
	logmsg(LOG_INFO, "Waiting for thread to join.");
	pthread_join(thread, NULL);
	
	break; // TODO remove
    }

    logmsg(LOG_INFO, "Closing distributor socket/thread.");
    close(sock);
    pthread_exit(NULL);
}

    
