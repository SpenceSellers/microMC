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

    Server *server = Server_create(NULL, 10);
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
	//logmsg(LOG_DEBUG, "Tick!");
	Server_tick(server);
	nanosleep(&sleeptime, NULL);
    }
    pthread_join(server->distributor_thread, NULL);
    logmsg(LOG_INFO, "Server shutting down!");
    
}

