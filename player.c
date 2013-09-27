#include "player.h"
#include "packets.h"
#include <stdlib.h>
#include "logging.h"
#include "map.h"

void Player_disconnect(Player *player){
    close(player->socket);
}

void Player_free(Player *player){
    close(player->socket);
    free(player->username);
}

void Player_send_keep_alive(Player *player){
    logmsg(LOG_DEBUG, "Sending keep alive to player.");
    Packet00KeepAlive info;
    info.id = 1; //Probably should be changed later
    size_t size;
    char *data = Packet00KeepAlive_encode(&info, &size);
    send(player->socket, data, size, 0);
    free(data);
}

void Player_send_block_change(Player *player, Block b, int x, int y, int z){
    Packet35BlockChange *packet = malloc(sizeof(packet));
    packet->x = x;
    packet->y = y;
    packet->z = z;
    packet->id = b.id;
    packet->metadata = b.metadata;
    
    size_t len;
    char *data = Packet35BlockChange_encode(packet, &len);
    logmsg(LOG_DEBUG, "Sending block change..");
    send(player->socket, data, len, 0);
    logmsg(LOG_DEBUG, "Done sending block change.");
}

    
void Player_break_block(Player *player, Server *s,  int x, int y, int z){
    logmsg(LOG_DEBUG, "Player has broken block!");
    Block air;
    air.id = 0;
    air.metadata = 0;
    //Map_set_block(s->map, air, x, y, z);

    Server_change_block(s, air, x, y, z);
}

