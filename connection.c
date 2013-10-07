#include "connection.h"
#include "logging.h"
#include "pthread.h"
#include "player.h"
#include "packets.h"
#include "server.h"
#include "map.h"
#include "inventory.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <limits.h>
#include <unistd.h>

#define BUFFERSIZE 1024

#define PORT "25565"
#define CONNECTION_BACKLOG 10

#include "playerconnectionhandlers.h"
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
    pthread_rwlock_rdlock(&server->state_lock);
    Player *player = handle_login(sock, server);
    pthread_rwlock_unlock(&server->state_lock);
    if (player == NULL){
	close(sock);
	pthread_exit(NULL);
    } else {
	pthread_rwlock_wrlock(&server->players_lock);
	Server_add_player(server, player);
	pthread_rwlock_unlock(&server->players_lock);
    }
    /* Now to send the chunks to the player. */
    pthread_rwlock_rdlock(&server->map_lock);
    send_all_chunks(player, server->map);
    pthread_rwlock_unlock(&server->map_lock);
   
    send_all_players(player, server);

    /*Player_give_slot(player, Slot_new_basic(3,20,0));
    Player_give_slot(player, Slot_new_basic(1,20,0));
    Player_give_slot(player, Slot_new_basic(3,5,0));*/
    
    // General play loop.
    while (1){
	ssize_t read = recv(sock, buffer, BUFFERSIZE, 0);
        if (read <= 0){
	    logmsg(LOG_INFO, "Socket has been closed! Removing player.");
	    close(sock);
	    pthread_rwlock_wrlock(&server->players_lock);
	    
	    Server_remove_player(server, player);
	    Player_free(player);
	    pthread_rwlock_unlock(&server->players_lock);
	    pthread_exit(NULL);
	    return NULL;
	}
        //printf("Packet of type: %d \n", buffer[0]);
	if (buffer[0] == PACKET_PLAYER_DIGGING){
	    Packet0EPlayerDigging *pack = Packet0EPlayerDigging_parse(buffer, read);
	    handle_player_digging(pack, player, server);
	    Packet0EPlayerDigging_free(pack);
	    
	} else if (buffer[0] == PACKET_CHAT_MESSAGE){
	    Packet03ChatMessage *pack = Packet03ChatMessage_parse(buffer, read);
	    handle_player_chat(pack, player, server);
	    Packet03ChatMessage_free(pack);
	    
	} else if (buffer[0] == PACKET_PLAYER_POSITION){
	    Packet0BPlayerPosition *pack =
		Packet0BPlayerPosition_parse(buffer, read);
	    handle_player_position(pack, player, server);
	    Packet0BPlayerPosition_free(pack);
	    
	} else if (buffer[0] == PACKET_PLAYER_BLOCK_PLACEMENT){
	    Packet0FPlayerBlockPlacement *pack =
		Packet0FPlayerBlockPlacement_parse(buffer, read);
	    handle_block_placement(pack, player, server);
	    Packet0FPlayerBlockPlacement_free(pack);
	} else if (buffer[0] == PACKET_HELD_ITEM_CHANGE){
	    Packet10HeldItemChange *pack = Packet10HeldItemChange_parse(buffer, read);
	    handle_item_change(pack, player, server);
	    Packet10HeldItemChange_free(pack);
	}
	
    }
    return NULL;
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

    
