#include "logging.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define LOGFMT_BUFFER_SIZE 512
pthread_mutex_t log_mutex;


void logmsg(int level, const char *message){
    if (level <= LOGGING_LEVEL){
	char *level_string;
	switch (level){
	case LOG_ERROR:
	    level_string = "[Error]";
	    break;
	case LOG_WARN:
	    level_string = "[WARN]";
	    break;
	case LOG_INFO:
	    level_string = "[INFO]";
	    break;
	case LOG_DEBUG:
	    level_string = "[DEBUG]";
	    break;
	}
	pthread_mutex_lock(&log_mutex);
	printf("%s %s \n", level_string, message);
	pthread_mutex_unlock(&log_mutex);
    }
}

void logfmt(int level, const char *fmt, ...){
    char buffer[LOGFMT_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, LOGFMT_BUFFER_SIZE, fmt, args);
    va_end(args);
    logmsg(level, buffer);
}

void logging_init(){
    pthread_mutex_init(&log_mutex, NULL);
}
