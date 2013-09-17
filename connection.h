#ifndef CONNECTION_H
#define CONNECTION_H
#include "server.h"

void *connection_thread(void *socket);

void *connection_distributor_thread(void *nothing);
#endif
