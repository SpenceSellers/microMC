#ifndef ENCODINGS_H
#define ENCODINGS_H
#include <stdlib.h>

char * decode_MCString(char *mcstring, size_t *read);

/*
 *  Encodes an ASCII string into Minecraft's UTF-16BE format.
 *  It has no unicode support at this time.
 */
char * encode_MCString(char *string, size_t *len);

void write_MCint(int i, char *place, size_t *len);

#endif
