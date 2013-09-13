#include "connection.h"
#include "logging.h"
#include "pthread.h"
#define BUFFERSIZE 1024

void *connection_thread(void *socket){
    int *sock = (int *) socket;
    logmsg(LOG_INFO, "Started new connection thread.");
    char buffer[BUFFERSIZE];
    while (1){
	int read = recv(*sock, buffer, BUFFERSIZE, NULL);
	if (read < 0){
	    logmsg(LOG_ERROR, "Reading from socket resulted in an error!");
	    close(socket);
	    pthread_exit(NULL);
	}
	buffer[read] = 0;
	printf("Read: %s \n", buffer);
	if (buffer[0] == 'q'){
	    logmsg(LOG_INFO, "Recieved a q. Quitting thread.");
	    close(*sock);
	    pthread_exit(NULL);
	}
    }
}
