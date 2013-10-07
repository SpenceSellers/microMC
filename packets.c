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
    
    packet->server_port = read_int(data + pos, &read);
    
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
 * Packet 0x03 Chat Message
 */
char * Packet03ChatMessage_encode(Packet03ChatMessage *data, size_t *len){
    char * packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;

    pos += write_char(PACKET_CHAT_MESSAGE, packet+pos);
    size_t stringwrote;
    char *mc_chatstring = encode_MCString(data->str, &stringwrote);
    memcpy(packet+pos, mc_chatstring, stringwrote);
    free(mc_chatstring);

    pos += stringwrote;

    *len = pos;
    return packet;
}
Packet03ChatMessage * Packet03ChatMessage_parse(char *data, size_t len){
    Packet03ChatMessage *chatpacket = malloc(sizeof(data));
    size_t pos = 0;
    if (read_char(data+pos, &pos) != PACKET_CHAT_MESSAGE){
	logmsg(LOG_WARN, "This is not a Chat message packet!");
	return NULL;
    }
    size_t read;
    char * chatstring = decode_MCString(data+pos, &read);
    chatpacket->str = chatstring;
    return chatpacket;
    
}
    
	    

void Packet03ChatMessage_free(Packet03ChatMessage *data){
    free(data->str);
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
 * Packet 0B Player Position
 */
Packet0BPlayerPosition * Packet0BPlayerPosition_parse(char *data, size_t len){
    Packet0BPlayerPosition *player_pos = malloc(sizeof(Packet0BPlayerPosition));
    size_t pos = 0;
    if (read_char(data + pos, &pos) != PACKET_PLAYER_POSITION){
	logmsg(LOG_WARN, "This is not a Player Position packet!");
	return NULL;
    }
    player_pos->x = read_double(data+pos, &pos);
    player_pos->y = read_double(data+pos, &pos);
    player_pos->stance = read_double(data+pos, &pos);
    player_pos->z = read_double(data+pos, &pos);
    player_pos->on_ground = read_char(data+pos, &pos);
    return player_pos;
}
void Packet0BPlayerPosition_free(Packet0BPlayerPosition *data){
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
 * Packet 0x0E Player Digging
 */

Packet0EPlayerDigging *Packet0EPlayerDigging_parse(char *data, size_t len){
    Packet0EPlayerDigging *packet =  malloc(sizeof(Packet0EPlayerDigging));
    size_t pos = 0;
    if (read_char(data + pos, &pos) != PACKET_PLAYER_DIGGING){
	logmsg(LOG_WARN, "This is not a player digging packet!");
	free(packet);
	return NULL;
    }
    
    packet->action = read_char(data + pos, &pos);
    packet->x = read_int(data+pos, &pos);
    packet->y = read_char(data+pos, &pos);
    packet->z = read_int(data+pos, &pos);
    packet->face = read_char(data+pos, &pos);


    return packet;
}
    
    
void Packet0EPlayerDigging_free(Packet0EPlayerDigging *data){
    free(data);
}
/*
 * Packet 0x0F Player Block Placement
 */
Packet0FPlayerBlockPlacement * Packet0FPlayerBlockPlacement_parse(char *data,
								  size_t len){
    size_t pos = 0;
    if (read_char(data + pos, &pos) != PACKET_PLAYER_BLOCK_PLACEMENT){
	logmsg(LOG_WARN, "This is not a block placement packet!");
	return NULL;
    }
    Packet0FPlayerBlockPlacement *pbp =
	malloc(sizeof(Packet0FPlayerBlockPlacement));
    pbp->x = read_int(data+pos, &pos);
    pbp->y = read_char(data+pos, &pos);
    pbp->z = read_int(data+pos, &pos);
    pbp->direction = read_char(data+pos, &pos);
    
    size_t slotread;
    pbp->held_item = Slot_read(data+pos, &slotread);
    pos += slotread;
    logmsg(LOG_DEBUG, "Done reading slot data!");
    
    pbp->c_posx = read_char(data+pos, &pos);
    pbp->c_posy = read_char(data+pos, &pos);
    pbp->c_posz = read_char(data+pos, &pos);
    logmsg(LOG_DEBUG, "Done reading block placement!");
    return pbp;
}

void Packet0FPlayerBlockPlacement_free(Packet0FPlayerBlockPlacement *data){
    Slot_free(data->held_item);
    free(data);
}
/*
 * Packet 0x10 Held Item Change
 */
Packet10HeldItemChange * Packet10HeldItemChange_parse(char *data, size_t len){
    size_t pos = 0;
    if (read_char(data+pos, &pos) != PACKET_HELD_ITEM_CHANGE){
	logmsg(LOG_WARN, "This is not a Held Item Change Packet!");
	return NULL;
    }
    Packet10HeldItemChange *itemchange = malloc(sizeof(Packet10HeldItemChange));
    itemchange->slot_id = read_short(data+pos, &pos);
    return itemchange;
}
    
void Packet10HeldItemChange_free(Packet10HeldItemChange *data){
    free(data);
}
/*
 * Packet 0x14 Spawn Named Entity
 */
char * Packet14SpawnNamedEntity_encode(Packet14SpawnNamedEntity *data,
				       size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;
    pos += write_char(PACKET_SPAWN_NAMED_ENTITY, packet+pos);
    pos += write_int(data->id, packet+pos);

    size_t stringwrote;
    char *mc_player_name = encode_MCString(data->player_name, &stringwrote);
    memcpy(packet+pos, mc_player_name, stringwrote);
    free(mc_player_name);
    pos += stringwrote;

    pos += write_int(data->x_fixedp, packet+pos);
    pos += write_int(data->y_fixedp, packet+pos);
    pos += write_int(data->z_fixedp, packet+pos);
    pos += write_char(data->yaw, packet+pos);
    pos += write_char(data->pitch, packet+pos);
    pos += write_short(data->held_item, packet+pos);
    pos += write_char(0, packet+pos); // TEMPORARY
    pos += write_char(0, packet+pos);
    pos += write_char(0x7F, packet+pos);

    *len = pos;
    return packet;
}
void Packet14SpawnNamedEntity_free(Packet14SpawnNamedEntity *data){
    free(data->player_name);
    free(data);
    // TODO add metadata
}
/*
 * Packet 0x1F Entity Relative Move
 */
char * Packet1FEntityRelativeMove_encode(Packet1FEntityRelativeMove *data,
					 size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;
    pos += write_char(PACKET_ENTITY_RELATIVE_MOVE, packet+pos);
    pos += write_int(data->id, packet+pos);
    pos += write_char(data->dx, packet+pos);
    pos += write_char(data->dy, packet+pos);
    pos += write_char(data->dz, packet+pos);

    *len = pos;
    return packet;
}
void Packet1FEntityRelativeMove_free(Packet1FEntityRelativeMove *data){
    free(data);
}

/*
 * Packet 0x22 Entity Teleport
 */
char * Packet22EntityTeleport_encode(Packet22EntityTeleport *data, size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;
    pos += write_char(PACKET_ENTITY_TELEPORT, packet+pos);
    pos += write_int(data->id, packet+pos);
    pos += write_int(data->x_f, packet+pos);
    pos += write_int(data->y_f, packet+pos);
    pos += write_int(data->z_f, packet+pos);
    pos += write_char(data->yaw, packet+pos);
    pos += write_char(data->pitch, packet+pos);
    *len = pos;
    return packet;
}
void Packet22EntityTeleport_free(Packet22EntityTeleport *data){
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
    pos += write_short(0, packet+pos); // Add bit map, unused here.
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
    //Block Id's
    for (i=0; i < 16*16*256; i++){
	pos += write_char(chunk->blocks[i].id, data+pos);
    }
    //Metadata
    for (i=0; i < (16*16*256); i+= 2){
	pos += write_char(
	    pack_halfchars(chunk->blocks[i].metadata,
			   chunk->blocks[i+1].metadata),
	    data + pos);
    }
    // Lighting
    for (i=0; i < (16*16*256); i+= 2){
	pos += write_char(255, data + pos);
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
    packet.bitmap = 65535; // All subchunks

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

/*
 * Packet 35 Block Change
 */

char * Packet35BlockChange_encode(Packet35BlockChange *data, size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;
    pos += write_char(PACKET_BLOCK_CHANGE, packet+pos);
    pos += write_int(data->x, packet+pos);
    pos += write_char(data->y, packet+pos);
    pos += write_int(data->z, packet+pos);
    pos += write_short(data->id, packet+pos);
    pos += write_char(data->metadata, packet+pos);

    *len = pos;
    return packet;
}

void Packet35BlockChange_free(Packet35BlockChange *data){
    free(data);
}
/*
 * Packet 0x67 Set Slot
 */
char * Packet67SetSlot_encode(Packet67SetSlot *data, size_t *len){
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE + Slot_encoded_size(data->slot));
    size_t pos = 0;
    pos += write_char(PACKET_SET_SLOT, packet+pos);
    pos += write_char(data->window_id, packet+pos);
    pos += write_short(data->slot_id, packet+pos);

    size_t slotlen;
    char *slotdata = Slot_encode(data->slot, &slotlen);
    memcpy(packet+pos, slotdata, slotlen);
    pos += slotlen;
    *len = pos;
    free(slotdata);
    return packet;
}
/*
 * Packet 0x68 Set Window Items
 */
char * Packet68SetWindowItems_encode(Packet68SetWindowItems *data, size_t *len){
    size_t size = 4; // 1 for ID, 1 for Window ID, 2 for length
    Slot *empty = Slot_new_empty();
    size_t emptysize = Slot_encoded_size(empty);
    free(empty);
    for (int i = 0; i < data->inv->size; i++){
	Slot *s = Inventory_get(data->inv, i);
	if (s == NULL){
	    size += emptysize;
	} else {
	    size += Slot_encoded_size(s);
	}
    }
    char *packet = malloc(sizeof(char) * size);
    size_t pos = 0;
    pos += write_char(PACKET_SET_WINDOW_ITEMS, packet+pos);
    pos += write_char(data->window_id, packet+pos);
    pos += write_short(data->inv->size, packet+pos);
    for (int i= 0; i < data->inv->size; i++){
	size_t slotlen;
	char * slotdata = Slot_encode(Inventory_get(data->inv, i), &slotlen);
	memcpy(packet+pos, slotdata, slotlen);
	pos += slotlen;
	free(slotdata);
    }
    *len = pos;
    return packet;
}
    
   
/*
 * Packet 0xFF Disconnect
 */

char *PacketFFDisconnect_encode(PacketFFDisconnect *data, size_t *len){
    if (1 + encoded_MCString_len(data->reason) > PACKET_BUFFER_SIZE){
	logmsg(LOG_WARN, "Packet buffer too small for disconnect packet!");
	return NULL;
    }
    char *packet = malloc(sizeof(char) * PACKET_BUFFER_SIZE);
    size_t pos = 0;
    pos += write_char(PACKET_DISCONNECT, packet+pos);
    
    size_t reason_len;
    char *mc_reason = encode_MCString(data->reason, &reason_len);
    memcpy(packet+pos, mc_reason, reason_len);
    
    pos += reason_len;
    free(mc_reason);

    *len = pos;
    return packet;
}

PacketFFDisconnect *PacketFFDisconnect_parse(char *data, size_t len){
    PacketFFDisconnect *packet =  malloc(sizeof(packet));
    size_t pos = 0;
    if (read_char(data + pos, &pos) != PACKET_DISCONNECT){
	logmsg(LOG_WARN, "This is not a disconnect packet!");
	return NULL;
    }
    size_t read;
    packet->reason = decode_MCString(data+pos, &read);
    return packet;
}

void PacketFFDisconnect_free(PacketFFDisconnect *data){
    free(data->reason);
    free(data);
}
