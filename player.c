#include "player.h"
#include "packets.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "logging.h"
#include "encodings.h"
#include "map.h"

void Player_disconnect(Player *player, char * reason){
    PacketFFDisconnect *disconnect = malloc(sizeof(PacketFFDisconnect));
    if (reason != NULL){
	disconnect->reason = strdup(reason);
    } else {
	disconnect->reason = strdup("Bye!");
    }
    size_t len;
    char *packet = PacketFFDisconnect_encode(disconnect, &len);
    printf("Disconnect of size: %d \n", len);
    send(player->socket, packet, len, 0);
    PacketFFDisconnect_free(disconnect);
    free(packet);
    close(player->socket);
}

void Player_free(Player *player){
    close(player->socket);
    free(player->username);
    free(player);
}

void Player_send_keep_alive(Player *player){
    Packet00KeepAlive info;
    info.id = 1; //Probably should be changed later
    size_t size;
    char *data = Packet00KeepAlive_encode(&info, &size);
    send(player->socket, data, size, 0);
    free(data);
}

void Player_send_block_change(Player *player, Block b, int x, int y, int z){
    Packet35BlockChange *packet = malloc(sizeof(Packet35BlockChange));
    packet->x = x;
    packet->y = y;
    packet->z = z;
    packet->id = b.id;
    packet->metadata = b.metadata;
    
    size_t len;
    char *data = Packet35BlockChange_encode(packet, &len);
    send(player->socket, data, len, 0);
    Packet35BlockChange_free(packet);
    free(data);
    
}

void Player_break_block(Player *player, Server *s,  int x, int y, int z){
    logmsg(LOG_DEBUG, "Player has broken block!");
    Block air;
    air.id = 0;
    air.metadata = 0;
    //Map_set_block(s->map, air, x, y, z);

    Server_change_block(s, air, x, y, z);
}

void Player_send_message(Player *player, char *msg){
    Packet03ChatMessage *chatmsg = malloc(sizeof(Packet03ChatMessage));
    chatmsg->str = fake_json_chat(msg);
    size_t len;
    
    char * packet = Packet03ChatMessage_encode(chatmsg, &len);

    send(player->socket, packet, len, 0);

    free(packet);
    Packet03ChatMessage_free(chatmsg);
}

void Player_set_position(Player *player, double x, double y, double z){	
    player->x = x;
    player->y = y;
    player->z = z;
}

void Player_send_new_player(Player *player, Player *newplayer){
    
    Packet14SpawnNamedEntity *pack = malloc(sizeof(Packet14SpawnNamedEntity));
    pack->id = newplayer->entity_id;
    pack->player_name = strdup(newplayer->username);
    pack->x_fixedp = to_fixed_point(newplayer->x);
    pack->y_fixedp = to_fixed_point(newplayer->y);
    pack->z_fixedp = to_fixed_point(newplayer->z);
    pack->yaw = 0;
    pack->pitch = 0;
    pack->held_item = 0;
    pack->metadata = 0x1F;
    size_t len;
    char *packet = Packet14SpawnNamedEntity_encode(pack, &len);
    send(player->socket, packet, len, 0);
    Packet14SpawnNamedEntity_free(pack);
    free(packet);
}

void Player_send_player_position(Player *player, Player *other){
    if (other->x - other->last_x == 0.0 &&
	other->y - other->last_y == 0.0 &&
	other->z - other->last_z == 0.0){
	return;
	}    
    if (abs(other->x - other->last_x) > 4 ||
	abs(other->y - other->last_y) > 4 ||
	abs(other->z - other->last_z) > 4){
	
	Packet22EntityTeleport *pack = malloc(sizeof(Packet22EntityTeleport));
	pack->id = other->entity_id;
	pack->x_f = to_fixed_point(other->x);
	pack->y_f = to_fixed_point(other->y);
	pack->z_f = to_fixed_point(other->z);
	pack->yaw = 0;
	pack->pitch = 0;
	size_t len;
	char *packet = Packet22EntityTeleport_encode(pack, &len);
	send(player->socket, packet, len, 0);
	Packet22EntityTeleport_free(pack);
	free(packet);
    } else {
	Packet1FEntityRelativeMove *pack =
	    malloc(sizeof(Packet1FEntityRelativeMove));
	pack->id = other->entity_id; //FIX
	pack->dx = to_fixed_point(other->x - other->last_x);
	pack->dy = to_fixed_point(other->y - other->last_y);
	pack->dz = to_fixed_point(other->z - other->last_z);
	size_t len;
	char *packet = Packet1FEntityRelativeMove_encode(pack, &len);
	send(player->socket, packet, len, 0);
	Packet1FEntityRelativeMove_free(pack);
	free(packet);
    }
}

void Player_tick_lastlocation(Player *player){
    player->last_x = player->x;
    player->last_y = player->y;
    player->last_z = player->z;
}
