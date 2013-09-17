#include "packets.h"

#include <stdlib.h>
#include <string.h>
#include "logging.h"

#define PACKET_BUFFER_SIZE 2048
void debug_print_hex_string(char *str, size_t len){
    int i;
    for (i=0; i < len; i++){
	printf("%x ", str[i]);
    }
    printf("\n");
}

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

Packet02Handshake * Packet02Handshake_parse(char *data, size_t length){
    logmsg(LOG_DEBUG, "Decoding Packet02Handshake");
    size_t pos = 0; //Current parsing position
    
    if (data[pos] != PACKET_HANDSHAKE){
	logmsg(LOG_WARN, "This is not a Handshake packet!");
	return NULL;
    }
    pos++;
    Packet02Handshake *packet = malloc(sizeof(Packet02Handshake));
    size_t read = 0; //Number of bytes last parsed

    packet->version = data[pos];
    pos++;
    
    packet->username = decode_MCString(data + pos, &read);
    pos += read;

    packet->server_name = decode_MCString(data+pos, &read);
    pos += read;
    
    packet->server_port = ntohl(*(int* )(data + pos));
    
    return packet;
}

void Packet02Handshake_free(struct Packet02Handshake *packet){
    free(packet->username);
    free(packet->server_name);
    free(packet);
}

// PACKET O1 LOGIN REQUEST

char * Packet01LoginRequest_encode(Packet01LoginRequest *data, size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;
    size_t wrote;
    packet[pos] = PACKET_LOGIN_REQUEST;
    pos++;

    write_MCint(data->entity_id, packet+pos, &wrote);
    pos+= wrote;
    
    char *mclevelstring = encode_MCString(data->level_type, &wrote);
    memcpy(packet + pos, mclevelstring, wrote);
    pos += wrote;

    packet[pos] = data->game_mode;
    pos++;

    packet[pos] = data->difficulty;
    pos++;

    /* The minecraft protocol has an unused value here.
       It used to be world height, but now the vanilla server
       just sends zero. */
    packet[pos] = 0;
    pos++;

    packet[pos] = data->max_players;
    pos++;

    *len = pos;

    return packet;
}
    

void Packet01LoginRequest_free(Packet01LoginRequest *data){
    free(data->level_type);
    free(data);
}

/*
 * Packet 0x06 Spawn Position
 */
char * Packet06SpawnPosition_encode(Packet06SpawnPosition *data, size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;
    size_t wrote;

    packet[pos] = PACKET_SPAWN_POSITION;
    pos++;

    write_MCint(data->x, packet + pos, &wrote);
    pos += wrote;

    write_MCint(data->y, packet + pos, &wrote);
    pos += wrote;

    write_MCint(data->z, packet + pos, &wrote);
    pos += wrote;

    *len = pos;
    return packet;

}

void Packet06SpawnPosition_free(Packet06SpawnPosition *data){
    free(data);
}

