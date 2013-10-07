#include "commands.h"
#include "logging.h"
#include "inventory.h"

#include <stdlib.h>
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

void command_spawn(Player *p, Server *s, char *cmd){
    Player_teleport(p, s->spawnx, s->spawny, s->spawnz);
}

void command_item(Player *p, Server *s, char *cmd){
    /* Ok, this function use a Goto for error handling.
     * Donald Knuth supports goto for this purpose.
     * The Linux Kernel uses goto for this purpose.
     * Read:
     * http://eli.thegreenplace.net/2009/04/27/using-goto-for-error-handling-in-c/
     */
    char *token;
    char *remainder;
    token = strtok_r(cmd, " ", &remainder);
    if (token == NULL) goto args_error;
    int id = atoi(token);
    
    token = strtok_r(NULL, " ", &remainder);
    if (token == NULL) goto args_error;
    int count = atoi(token);

    Slot *slot = Slot_new_basic(id, count, 0);
    Player_give_slot(p, slot);
    free(slot);
    return;

args_error:
    Player_send_message(p, "Invalid number of arguments!");
    return;
}

    
void handle_command(Player *p, Server *s, char *cmd){
    char *first;
    char *remainder;
    first = strtok_r(cmd, " ", &remainder);

    if (strcmp(first, "/kickme") == 0) command_kick_me(p, s, remainder);
    else if (strcmp(first, "/givestuff") == 0) command_give_stuff(p, s, remainder);
    else if (strcmp(first, "/spawn") == 0) command_spawn(p, s, remainder);
    else if (strcmp(first, "/i") == 0) command_item(p, s, remainder);
    else Player_send_message(p, "Unrecognized command!");
    
}
