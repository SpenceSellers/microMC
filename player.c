#include "player.h"

void Player_disconnect(Player *player){
    close(player->socket);
}
