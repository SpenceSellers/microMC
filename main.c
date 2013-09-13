#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

#include <pthread.h>
#include "logging.h"

#define PORT "1234"
#define CONNECTION_BACKLOG 10
int main(){
    logging_init();
    logmsg(LOG_INFO, "Server started and logging started.");

    exit(0);
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
    }
    listen(sockfd, CONNECTION_BACKLOG);

    addr_size = sizeof their_addr;
    while (1){
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    }
    
    close(sockfd);
}
