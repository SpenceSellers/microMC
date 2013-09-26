#include "encodings.h"
#include "logging.h"

#include <string.h>
#include <zlib.h>

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

size_t write_int(int i, char *place){
    int bige_int = htonl(i);
    *(int*) place = bige_int;
    return 4;
}

size_t write_short(short s, char *place){
    short flipped = htons(s);
    *(short*) place = flipped;
    return 2;
}
size_t write_char(char c, char *place){
    *place = c;
    return 1;
}

size_t write_float(float f, char *place){
    float flipped = swap_float_endian(f);
    *(float*) place = flipped;
    return 4;
}

size_t write_double(double d, char *place){
    double flipped = swap_double_endian(d);
    *(double*) place = flipped;
    return 8;
}

size_t write_compressed(char *data, size_t len, char *place, size_t maxlen){
    size_t written = maxlen;
    int e = compress(place, &written, data, len);
    if (e == Z_MEM_ERROR){
	logmsg(LOG_ERROR, "Not enough memory given to compress data!");
    }
    return written;
}
    
int read_int(char *place, size_t *read){
    if (read) *read += 4;
    return ntohl(*( (int*) place));
}
short read_short(char *place, size_t *read){
    if (read) *read += 2;
    return ntohs(*( (short*) place));
}
char read_char(char *place, size_t *read){
    if (read) *read += 1;
    return *place;
}
float read_float(char *place, size_t *read){
    float f = *(float*) place;
    if (read) *read += 4;
    return swap_float_endian(f);
}

double read_double(char *place, size_t *read){
    double d = *(double*) place;
    if (read) *read += 8;
    return swap_double_endian(d);
}

float swap_float_endian(float f){
    float result;
    char *fchars = (char*) &f;
    char *rchars = (char*) &result;

    rchars[0] = fchars[3];
    rchars[1] = fchars[2];
    rchars[2] = fchars[1];
    rchars[3] = fchars[0];

    return result;
}


double swap_double_endian(double f){
    double result;
    char *fchars = (char*) &f;
    char *rchars = (char*) &result;

    rchars[0] = fchars[7];
    rchars[1] = fchars[6];
    rchars[2] = fchars[5];
    rchars[3] = fchars[4];
    rchars[4] = fchars[3];
    rchars[5] = fchars[2];
    rchars[6] = fchars[1];
    rchars[7] = fchars[0];

    return result;
}

char pack_halfchars(char a, char b){
    return (a << 4) & b;
}
