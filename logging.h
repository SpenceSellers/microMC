#ifndef LOGGING_H
#define LOGGING_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define LOG_ERROR 0 // Critical Failures, or impending critical failures.
#define LOG_WARN 1 // Things which could create a problem
#define LOG_INFO 2 // Mildly interesting events
#define LOG_DEBUG 3 // Fine grained stuff which

// The current logging level.
#define LOGGING_LEVEL 3

void logmsg(int level, char *message); 

void logging_init();
#endif
