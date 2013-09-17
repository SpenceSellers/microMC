#include "logging.h"

#include <string.h>
char * decode_MCString(char *mcstring, size_t *read){
    logmsg(LOG_DEBUG, "Parsing a Minecraft String.");
    // Endianness conversion...
    short code_unit_length = ntohs(*((short*) (mcstring + 0)));
    /* Since Minecraft's strings are sent as UTF 16,
       the length in bytes is just twice the code unit length.*/
    size_t byte_length = code_unit_length * 2;
    
    // Making sure to keep enough space for the null terminator.
    size_t cstring_length = code_unit_length + 1;

    char *cstring = malloc(sizeof(char) * cstring_length);
    size_t cspos = 0;
    size_t i;
    for(i=2; i<byte_length+2;i++){
	if (mcstring[i] != 0){
	    cstring[cspos] = mcstring[i];
	    cspos++;
	}
    }
    cstring[cspos] = 0; // Null terminator

    if (read != NULL){
	*read = byte_length + 2;
    }
    
    logmsg(LOG_DEBUG, "Done parsing string!");
    return cstring;
}


char * encode_MCString(char *string, size_t *len){
    logmsg(LOG_DEBUG, "Encoding a MCstring");
    size_t stringlen = strlen(string);
    size_t mcstrlen = (stringlen * 2) + 2; //Plus Two for size short.
    char *mcstring = malloc(sizeof(char) * mcstrlen);

    *(short*) mcstring = htons(stringlen);

    int i;
    for (i=2; i < mcstrlen; i++){
	if (i % 2 == 0) {mcstring[i] = 0;}
	else { mcstring[i] = string[(i-3)/2];}
    }
    if (len != NULL){
	*len = mcstrlen;
    }
    return mcstring;
}

void write_MCint(int i, char *place,  size_t *len){
    int bige_int = htonl(i);

    *(int*) place = bige_int;
    *len = 4;
}
