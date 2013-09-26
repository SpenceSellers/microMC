#include "player.h"
#include "packets.h"
#include <stdlib.h>
#include "logging.h"
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

