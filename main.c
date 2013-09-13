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

#define PORT "1234"
#define CONNECTION_BACKLOG 10
int main(){
    logging_init();
    logmsg(LOG_INFO, "Server started and logging started.");
    
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int new_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &res);

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int binderr = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if (binderr == -1){
	logmsg(LOG_ERROR, "Unable to bind socket!");
	exit(1);
    }
    listen(sockfd, CONNECTION_BACKLOG);

    addr_size = sizeof their_addr;
    while (1){
	logmsg(LOG_DEBUG, "Main Thread waiting on new connection.");
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
	
	logmsg(LOG_INFO, "Accepted connection. Starting new thread!");
	pthread_t *thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(thread, &attr, &connection_thread, &new_fd);
	pthread_attr_destroy(&attr);
	
	logmsg(LOG_INFO, "Waiting for thread to join.");
	pthread_join(*thread, NULL);
	
	break; // TODO remove
    }
    logmsg(LOG_INFO, "Closing main socket.");
    close(sockfd);
    logmsg(LOG_INFO, "Closing!");
}
