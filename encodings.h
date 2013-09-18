#ifndef ENCODINGS_H
#define ENCODINGS_H
#include <stdlib.h>

char * decode_MCString(char *mcstring, size_t *read);

/*
 *  Encodes an ASCII string into Minecraft's UTF-16BE format.
 *  It has no unicode support at this time.
 */
char * encode_MCString(char *string, size_t *len);

size_t write_int(int i, char *place);
size_t write_short(short s, char *place);
size_t write_char(char c, char *place);
size_t write_float(float f, char *place);
size_t write_double(double d, char *place);

float swap_float_endian(float f);
double swap_double_endian(double f);
#endif
