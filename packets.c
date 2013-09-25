#include "packets.h"

#include <stdlib.h>
#include <string.h>
#include "logging.h"
#include "encodings.h"

#define PACKET_BUFFER_SIZE 2048
#define CHUNK_PACKET_BUFFER_SIZE 200000
void debug_print_hex_string(char *str, size_t len){
    int i;
    for (i=0; i < len; i++){
	printf("%x ", str[i]);
    }
    printf("\n");
}
char * Packet00KeepAlive_encode(Packet00KeepAlive *data, size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    
    size_t pos = 0;
    pos += write_char(PACKET_KEEP_ALIVE, packet+pos);
    pos += write_int(data->id, packet+pos);
    *len = pos;
    return packet;
}
void Packet00KeepAlive_free(Packet00KeepAlive *data){
    free(data);
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
    pos += write_char(PACKET_LOGIN_REQUEST, packet + pos);

    pos += write_int(data->entity_id, packet+pos);
    
    char *mclevelstring = encode_MCString(data->level_type, &wrote);
    memcpy(packet + pos, mclevelstring, wrote);
    free(mclevelstring);
    
    pos += wrote;

    pos += write_char(data->game_mode, packet+pos);

    pos += write_char(data->dimension, packet+pos);
    pos += write_char(data->difficulty, packet+pos);

    /* The minecraft protocol has an unused value here.
       It used to be world height, but now the vanilla server
       just sends zero. */
    pos += write_char(0, packet+pos);

    pos += write_char(data->max_players, packet+pos);

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
    
    pos += write_char(PACKET_SPAWN_POSITION, packet+pos);
    pos += write_int(data->x, packet + pos);
    pos += write_int(data->y, packet + pos);
    pos += write_int(data->z, packet + pos);

    *len = pos;
    return packet;

}

void Packet06SpawnPosition_free(Packet06SpawnPosition *data){
    free(data);
}

/*
 * Packet 0x0D Player Position and Look
 */

char * Packet0DPlayerPositionAndLook_encode(Packet0DPlayerPositionAndLook *data,
					    size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;
    pos += write_char(PACKET_PLAYER_POSITION_AND_LOOK, packet+pos);
    pos += write_double(data->x, packet+pos);
    pos += write_double(data->y_stance, packet+pos);
    pos += write_double(data->stance_y, packet+pos);
    pos += write_double(data->z, packet+pos);
    pos += write_float(data->yaw, packet+pos);
    pos += write_float(data->pitch, packet+pos);
    pos += write_char(data->on_ground, packet+pos);

    *len = pos;
    return packet;
    
}

void Packet0DPlayerPositionAndLook_free(Packet0DPlayerPositionAndLook *data){
    free(data);
}
    
/* 
 * Packet 33 Chunk Data
 */
char * Packet33ChunkData_encode(Packet33ChunkData *data, size_t *len){
    if (data->compressed_size + 18 > CHUNK_PACKET_BUFFER_SIZE){
	logmsg(LOG_ERROR, "Chunk data is too big to fit in packet buffer!");
	return NULL;
    }
    
    char *packet = malloc(sizeof(char) * CHUNK_PACKET_BUFFER_SIZE);
    size_t pos = 0;
    pos += write_char(PACKET_CHUNK_DATA, packet+pos);
    pos += write_int(data->x, packet+pos);
    pos += write_int(data->z, packet+pos);
    pos += write_char(data->continuous, packet+pos);
    pos += write_short(data->bitmap, packet+pos);
    pos += write_int(data->compressed_size, packet+pos);
    memcpy(packet + pos, data->compressed_data, data->compressed_size);
    pos += data->compressed_size;

    *len = pos;
    return packet;
    
}

void Packet33ChunkData_free(Packet33ChunkData *data){
    free(data->compressed_data);
    free(data);
}

char *construct_chunk_data(Chunk *chunk, size_t *len){
    char * data = malloc(sizeof(char) * CHUNK_PACKET_BUFFER_SIZE);
    size_t pos = 0;
    int i;
    for (i=0; i < 16*16*256; i++){
	pos += write_char(chunk->blocks[i].id, data+pos);
    }
    for (i=0; i < (16*16*256)/2; i+= 2){
	pos += write_char(
	    pack_halfchars(chunk->blocks[i].metadata,
			   chunk->blocks[i+1].metadata),
	    data + pos);
    }
    for (i=0; i < (16*16*256)/2; i+= 2){
	pos += write_char(17, data + pos);
    }
    // We can skip the Add array.
    for (i=0; i < (16*16); i++){
	pos += write_char(chunk->biome_data[i], data+pos);
    }

    *len = pos;
    return data;
}

char *Packet33ChunkData_construct(Chunk *chunk, int x, int z, size_t *len){
    logmsg(LOG_INFO, "Constructing packet.");
    size_t chunkdata_len;
    char *chunkdata = construct_chunk_data(chunk, &chunkdata_len);

    Packet33ChunkData packet;
    packet.x = x;
    packet.z = z;
    packet.continuous = 1;
    packet.bitmap = 15; // All subchunks

    size_t comp_len = 0;

    char *compressed_data = malloc(sizeof(char) * CHUNK_PACKET_BUFFER_SIZE);
    comp_len = write_compressed(
	chunkdata, chunkdata_len, compressed_data, CHUNK_PACKET_BUFFER_SIZE);
    packet.compressed_data = compressed_data;
    packet.compressed_size = comp_len;
    free(chunkdata);
    
    char *final = Packet33ChunkData_encode(&packet, len);

    free(compressed_data);
    logmsg(LOG_INFO, "Done constructing.");
    return final;
    
}
