#include "logging.h"

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
	printf("%s %s \n", level_string, message);
    }
}
