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

#define DEFAULT_X_SIZE 5
#define DEFAULT_Z_SIZE 5

// Global variable for signal handling only!
static Server *sigint_server;

static void catch_sigint(int signal){
    logmsg(LOG_WARN, "Shutting down due to quit signal!");
    pthread_rwlock_wrlock(&sigint_server->players_lock);
    Server_shutdown(sigint_server);
    pthread_rwlock_unlock(&sigint_server->players_lock);
    exit(0);
    
}
int main(int argc, char *argv[]){
    logging_init();
    logmsg(LOG_INFO, "Server started and logging started.");
    int xsize = DEFAULT_X_SIZE;
    int zsize = DEFAULT_Z_SIZE;
    char *filename = NULL;
    char *out_name = NULL;
    for(int i=0; i < argc; i++){
	if (strcmp("-s", argv[i])== 0){
	    xsize = atoi(argv[i+1]);
	    zsize = atoi(argv[i+2]);
	    i += 2;
	} else if (strcmp("-f", argv[i]) == 0){
	    filename = argv[i+1];
	} else if (strcmp("-o", argv[i]) == 0){
	    out_name = argv[i+1];
	}
    }
    
    Map *map;
    if (filename == NULL){
	map = Map_new_air(xsize,zsize);
	Block b;
	b.id = 1;
	b.metadata = 0;
    
	Map_set_below(map, b, 60);
    
	b.id=20; map->chunks[0]->blocks[0] = b; //Debugging

	if (out_name != NULL){
	    filename = out_name;
	}
    } else {
	map = Map_read(filename);
    }
    
    Server *server = Server_create(map, 10);
    sigint_server = server;
    server->is_running = 1;
    server->filename = filename;
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

    return 0;
}

