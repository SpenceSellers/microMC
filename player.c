#include "player.h"
#include <stdlib.h>

void Player_disconnect(Player *player){
    close(player->socket);
}

void Player_free(Player *player){
    close(player->socket);
    free(player->username);
}

