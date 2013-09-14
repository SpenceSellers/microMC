#include "connection.h"
#include "logging.h"
#include "pthread.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#define BUFFERSIZE 1024

#define PORT "1234"
#define CONNECTION_BACKLOG 10

void *connection_thread(void *vsock){
    logmsg(LOG_INFO, "Started new connection thread.");
    int *sock = (int *) vsock;
    char buffer[BUFFERSIZE];
    while (1){
	int read = recv(*sock, buffer, BUFFERSIZE, 0);
	if (read <= 0){
	    logmsg(LOG_INFO, "Socket has been closed!");
	    close(*sock);
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
