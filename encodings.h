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
size_t write_compressed(char *data, size_t len, char *place, size_t maxlen);

/* These functions minecraft Big-Endian data from a char string.
 * the read argument will be ADDED to. Pass in a variable
 * set to zero if you want to use it like normal. Otherwise
 * You can use it to tally up where you are in the string.
 */
int read_int(char *place, size_t *read);
short read_short(char *place, size_t *read);
char read_char(char *place, size_t *read);
float read_float(char *place, size_t *read);
double read_double(char *place, size_t *read);


float swap_float_endian(float f);
double swap_double_endian(double f);

char pack_halfchars(char a, char b);

char * fake_json_chat(char *str);
#endif
