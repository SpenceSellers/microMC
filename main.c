#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

#include <pthread.h>
#include "logging.h"
#include "connection.h"


int main(){
    logging_init();
    logmsg(LOG_INFO, "Server started and logging started.");

    pthread_t distributor;
    pthread_create(&distributor, NULL, &connection_distributor_thread, NULL);
    pthread_join(distributor, NULL);
    logmsg(LOG_INFO, "Server shutting down!");
    
}
