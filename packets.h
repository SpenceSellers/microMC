#ifndef PACKETS_H
#define PACKETS_H

#include <stdlib.h>
#include "player.h"
#include "map.h"

#define PACKET_KEEP_ALIVE 0x00
#define PACKET_LOGIN_REQUEST 0x01
#define PACKET_HANDSHAKE 0x02
#define PACKET_SPAWN_POSITION 0x06
#define PACKET_PLAYER_POSITION_AND_LOOK 0x0D
#define PACKET_CHUNK_DATA 0x33

void debug_print_hex_string(char *str, size_t len);
typedef struct Packet00KeepAlive {
    int id;
} Packet00KeepAlive;

char * Packet00KeepAlive_encode(Packet00KeepAlive *data, size_t *len);
void Packet00KeepAlive_free(Packet00KeepAlive *data);
    
typedef struct Packet01LoginRequest {
    int entity_id;
    char *level_type;
    char game_mode;
    char dimension;
    char difficulty;
    char max_players;
} Packet01LoginRequest;

char * Packet01LoginRequest_encode(Packet01LoginRequest *data, size_t *len);
void Packet01LoginRequest_free(Packet01LoginRequest *data);

typedef struct Packet02Handshake {
    char version;
    char *username;
    char *server_name;
    int server_port;
} Packet02Handshake;

Packet02Handshake * Packet02Handshake_parse(char *data, size_t length);

typedef struct Packet06SpawnPosition {
    int x;
    int y;
    int z;
} Packet06SpawnPosition;

char * Packet06SpawnPosition_encode(Packet06SpawnPosition *data, size_t *len);
void Packet06SpawnPosition_free(Packet06SpawnPosition *data);

typedef struct Packet0DPlayerPositionAndLook {
    double x;
    double y_stance; /* These variables are swapped depending on */
    double stance_y; /* the direction the packet is traveling... */
    double z;
    float yaw;
    float pitch;
    char on_ground;
} Packet0DPlayerPositionAndLook;

char * Packet0DPlayerPositionAndLook_encode(Packet0DPlayerPositionAndLook *data,
					    size_t *len);
void Packet0DPlayerPositionAndLook_free(Packet0DPlayerPositionAndLook *data);

typedef struct Packet33ChunkData {
    int x;
    int z;
    char continuous;
    unsigned short bitmap;
    int compressed_size;
    char *compressed_data;
} Packet33ChunkData;

char * Packet33ChunkData_encode(Packet33ChunkData *data, size_t *len);
void Packet33ChunkData_free(Packet33ChunkData *data);
char *Packet33ChunkData_construct(Chunk *chunk, int x, int z, size_t *len);

#endif
