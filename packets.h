#ifndef PACKETS_H
#define PACKETS_H

#include <stdlib.h>
#include "player.h"

#define PACKET_LOGIN_REQUEST 0x01
#define PACKET_HANDSHAKE 0x02
#define PACKET_SPAWN_POSITION 0x06
#define PACKET_PLAYER_POSITION_AND_LOOK 0x0D

void debug_print_hex_string(char *str, size_t len);

typedef struct Packet01LoginRequest {
    int entity_id;
    char *level_type;
    char game_mode;
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
#endif
