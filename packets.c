#include "packets.h"

#include <stdlib.h>
#include "logging.h"

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
	    
    
struct Packet02Handshake {
    char version;
    char *username;
    char *server_name;
    int server_port;
};

struct Packet02Handshake * Packet02Handshake_parse(char *data, size_t length){
    logmsg(LOG_DEBUG, "Decoding Packet02Handshake");
    size_t pos = 0; //Current parsing position
    
    if (data[pos] != PACKET_HANDSHAKE){
	logmsg(LOG_WARN, "This is not a Handshake packet!");
	return NULL;
    }
    pos++;
    struct Packet02Handshake *packet = malloc(sizeof(struct Packet02Handshake));
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
Player * packet_handle02handshake(int socket, char *data, size_t length){
    logmsg(LOG_DEBUG, "Handling Packet02Handshake");
    struct Packet02Handshake *packet = Packet02Handshake_parse(data, length);

    printf("Version: %d\n", packet->version);
    printf("Username: %s\n", packet->username);
    printf("Server Name: %s\n", packet->server_name);
    printf("server port: %d\n", packet->server_port);
    
    
    return NULL;

}
