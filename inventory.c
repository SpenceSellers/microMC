#include "inventory.h"
#include <stdio.h>
#include <string.h>
#include "logging.h"
#include "encodings.h"

Slot * Slot_read(char *data, size_t *read){
    logmsg(LOG_DEBUG, "Reading slot.");
    Slot *slot = malloc(sizeof(Slot));
    size_t pos = 0;
    slot->id = read_short(data+pos, &pos);
    printf("Item ID is %d\n", slot->id);
    if (slot->id == -1){
	slot->count = 0;
	slot->damage = 0;
	slot->nbt_len = -1;
	slot->nbt = NULL;
	*read = pos;
	return slot;
    }
    printf("Continuing!\n");
    slot->count = read_char(data+pos, &pos);
    slot->damage = read_short(data+pos, &pos);
    slot->nbt_len = read_short(data+pos, &pos);
    printf("NBT len is %d \n", slot->nbt_len);
    if (slot->nbt_len == -1){
	slot->nbt = NULL;
	*read = pos;
	return slot;
    }

    slot->nbt = malloc(sizeof(char) * slot->nbt_len);
    memcpy(slot->nbt, data+pos, slot->nbt_len);

    *read = pos;
    return slot;
}

void Slot_free(Slot *slot){
    if (slot->nbt != NULL) free(slot->nbt);
    free(slot);
}

char Slot_is_empty(Slot *slot){
    if (slot->id == -1){
	return 1;
    } else {
	return 0;
    }
}
