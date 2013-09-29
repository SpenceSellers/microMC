#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>

#include <pthread.h>
#include "logging.h"
#include "connection.h"
#include "server.h"
#include "map.h"

// Global variable for signal handling only!
static Server *sigint_server;

static void catch_sigint(int signal){
    logmsg(LOG_WARN, "Shutting down due to quit signal!");
    Server_shutdown(sigint_server);
    exit(0);
    
}
int main(){
    logging_init();
    logmsg(LOG_INFO, "Server started and logging started.");
    Map *map = Map_new_air(5,5);
    Block b;
    b.id = 1;
    b.metadata = 0;
    
    Map_set_below(map, b, 60);
    
    b.id=20; map->chunks[0]->blocks[0] = b; //Debugging
    
    Server *server = Server_create(map, 10);
    sigint_server = server;
    server->is_running = 1;
    signal(SIGINT, catch_sigint);
    
    pthread_create(&(server->distributor_thread),
		   NULL, &connection_distributor_thread, server);
    
    logmsg(LOG_INFO, "Distributor thread started.");

    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = 50000000; // 50 ms.
    while (server->is_running){
	pthread_rwlock_wrlock(&server->state_lock);
	pthread_rwlock_wrlock(&server->players_lock);
	Server_tick(server);
	pthread_rwlock_unlock(&server->state_lock);
	pthread_rwlock_unlock(&server->players_lock);
	nanosleep(&sleeptime, NULL);
    }
    pthread_join(server->distributor_thread, NULL);
    logmsg(LOG_INFO, "Server shutting down!");
    
}

