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
    
    pthread_rwlock_wrlock(&p->lock);
    Player_give_slot(p, slot);
    pthread_rwlock_unlock(&p->lock);
    
    Slot_free(slot);
}

void command_spawn(Player *p, Server *s, char *cmd){
    pthread_rwlock_wrlock(&p->lock);
    Player_teleport(p, s->spawnx, s->spawny, s->spawnz);
    pthread_rwlock_unlock(&p->lock);
}

void command_item(Player *p, Server *s, char *cmd){
    int id;
    int count;
    int metadata;
    int read = sscanf(cmd, "%d %d %d", &id, &count, &metadata);
    Slot *slot;
    if (read == 1){
	slot = Slot_new_basic(id, 64, 0);
    } else if (read == 2){
	slot = Slot_new_basic(id, count, 0);
    } else if (read == 3){
	slot = Slot_new_basic(id, count, metadata);
    } else {
	Player_send_message(p, "Invalid number of arguments!");
	return;
    }
    Player_give_slot(p, slot);
    free(slot); 
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
