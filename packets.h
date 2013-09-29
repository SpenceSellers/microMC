#ifndef PACKETS_H
#define PACKETS_H

#include <stdlib.h>
#include "player.h"
#include "map.h"
#include "inventory.h"
#define PACKET_KEEP_ALIVE 0x00
#define PACKET_LOGIN_REQUEST 0x01
#define PACKET_HANDSHAKE 0x02
#define PACKET_CHAT_MESSAGE 0x03
#define PACKET_SPAWN_POSITION 0x06
#define PACKET_PLAYER_POSITION 0x0B
#define PACKET_PLAYER_POSITION_AND_LOOK 0x0D
#define PACKET_PLAYER_DIGGING 0x0E
#define PACKET_PLAYER_BLOCK_PLACEMENT 0x0F
#define PACKET_SPAWN_NAMED_ENTITY 0x14
#define PACKET_ENTITY_RELATIVE_MOVE 0x1F
#define PACKET_ENTITY_TELEPORT 0x22
#define PACKET_CHUNK_DATA 0x33
#define PACKET_BLOCK_CHANGE 0x35
#define PACKET_DISCONNECT 0xFF

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

typedef struct Packet03ChatMessage {
    char *str;
} Packet03ChatMessage;

char * Packet03ChatMessage_encode(Packet03ChatMessage *data, size_t *len);
Packet03ChatMessage * Packet03ChatMessage_parse(char *data, size_t len);
void Packet03ChatMessage_free(Packet03ChatMessage *data);

/*
 * Packet 06 Spawn Position
 */
typedef struct Packet06SpawnPosition {
    int x;
    int y;
    int z;
} Packet06SpawnPosition;

char * Packet06SpawnPosition_encode(Packet06SpawnPosition *data, size_t *len);
void Packet06SpawnPosition_free(Packet06SpawnPosition *data);

/*
 * Packet 0B Player Position
 */
typedef struct Packet0BPlayerPosition{
    double x;
    double y;
    double stance;
    double z;
    char on_ground;
} Packet0BPlayerPosition;

Packet0BPlayerPosition * Packet0BPlayerPosition_parse(char *data, size_t len);
void Packet0BPlayerPosition_free(Packet0BPlayerPosition *data);
    
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
/*
 * Packet 0x0E Player Digging
 */
typedef struct Packet0EPlayerDigging {
    char action;
    int x;
    char y;
    int z;
    char face;
} Packet0EPlayerDigging;

Packet0EPlayerDigging *Packet0EPlayerDigging_parse(char *data, size_t len);

void Packet0EPlayerDigging_free(Packet0EPlayerDigging *data);
/*
 * Packet 0x0F Player Block Placement
 */
typedef struct Packet0FPlayerBlockPlacement {
    int x;
    unsigned char y;
    int z;
    char direction;
    Slot *held_item;
    // Crosshair positions on block.
    char c_posx;
    char c_posy;
    char c_posz;
} Packet0FPlayerBlockPlacement;

Packet0FPlayerBlockPlacement * Packet0FPlayerBlockPlacement_parse(char *data,
								  size_t len);
void Packet0FPlayerBlockPlacement_free(Packet0FPlayerBlockPlacement *data);
/*
 * Packet 0x14 Spawn Named Entity
 */
typedef struct Packet14SpawnNamedEntity {
    int id;
    char *player_name;
    int x_fixedp;
    int y_fixedp;
    int z_fixedp;
    char yaw;
    char pitch;
    short held_item; // Defaults to zero!
    char metadata; // TEMPORARY
} Packet14SpawnNamedEntity;

char * Packet14SpawnNamedEntity_encode(Packet14SpawnNamedEntity *data, size_t *len);
void Packet14SpawnNamedEntity_free(Packet14SpawnNamedEntity *data);
    
/*
 * Packet 0x1F Entity Relative Move
 */
typedef struct Packet1FEntityRelativeMove {
    int id;
    char dx;
    char dy;
    char dz;
} Packet1FEntityRelativeMove;

char * Packet1FEntityRelativeMove_encode(Packet1FEntityRelativeMove *data,
					 size_t *len);
void Packet1FEntityRelativeMove_free(Packet1FEntityRelativeMove *data);

/*
 * Packet 0x1F Entity Teleport
 */
typedef struct Packet22EntityTeleport {
    int id;
    int x_f;
    int y_f;
    int z_f;
    char yaw;
    char pitch;
} Packet22EntityTeleport;

char * Packet22EntityTeleport_encode(Packet22EntityTeleport *data, size_t *len);
void Packet22EntityTeleport_free(Packet22EntityTeleport *data);

/*
 * Packet 0x33 Chunk Data
 */
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
/*
 * Packet 0x33 Block Change
 */
typedef struct Packet35BlockChange {
    int x;
    char y;
    int z;
    short id;
    char metadata;
} Packet35BlockChange;

char * Packet35BlockChange_encode(Packet35BlockChange *data, size_t *len);
void Packet35BlockChange_free(Packet35BlockChange *data);

/*
 * Packet 0xFF Disconnect
 */

typedef struct PacketFFDisconnect {
    char *reason;
} PacketFFDisconnect;

char *PacketFFDisconnect_encode(PacketFFDisconnect *data, size_t *len);
PacketFFDisconnect *PacketFFDisconnect_parse(char *data, size_t len);
void PacketFFDisconnect_free(PacketFFDisconnect *data);
#endif
