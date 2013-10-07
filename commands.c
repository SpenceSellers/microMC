#include "commands.h"
#include "logging.h"
#include "inventory.h"

#include <string.h>
#include <pthread.h>


void command_kick_me(Player *p, Server *s, char *cmd){
    Player_disconnect(p, "Get outa here!");
}

void command_give_stuff(Player *p, Server *s, char *cmd){
    Slot *slot = Slot_new_basic(5, 64, 0);
    Player_give_slot(p, slot);
    Slot_free(slot);
}

void handle_command(Player *p, Server *s, char *cmd){
    char *first;
    char *remainder;
    first = strtok_r(cmd, " ", &remainder);

    if (strcmp(first, "/kickme") == 0) command_kick_me(p, s, remainder);
    else if (strcmp(first, "/givestuff") == 0) command_give_stuff(p, s, remainder);
    
}
