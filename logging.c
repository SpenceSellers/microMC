#include "logging.h"

pthread_mutex_t log_mutex;


void logmsg(int level, char *message){
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

void logging_init(){
    pthread_mutex_init(&log_mutex, NULL);
}
