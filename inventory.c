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
    if (slot->id == -1){
	slot->count = 0;
	slot->damage = 0;
	slot->nbt_len = -1;
	slot->nbt = NULL;
	*read = pos;
	return slot;
    }
    slot->count = read_char(data+pos, &pos);
    slot->damage = read_short(data+pos, &pos);
    slot->nbt_len = read_short(data+pos, &pos);
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

char * Slot_encode(Slot *slot, size_t *len){
    logmsg(LOG_DEBUG, "Encoding slot.");
    if (slot == NULL){
	Slot *emptyslot = Slot_new_empty();
        char *data = Slot_encode(emptyslot, len);
	Slot_free(emptyslot);
	return data;
    }
	
    char *data;
    size_t pos = 0;
    if (Slot_is_empty(slot)){
	data = malloc(sizeof(char) * SIZE_SHORT);
	pos += write_short(slot->id, data + pos);
	*len = pos;
	return data;
    }

    if (slot->nbt_len == -1){
	data = malloc(sizeof(char) *
		      (SIZE_SHORT + SIZE_BYTE + SIZE_SHORT + SIZE_SHORT));
    } else {
	data = malloc(sizeof(char) *
		      (SIZE_SHORT + SIZE_BYTE + SIZE_SHORT + SIZE_SHORT +
		       slot->nbt_len));
    }
    
    pos += write_short(slot->id, data+pos);
    pos += write_char(slot->count, data+pos);
    pos += write_short(slot->damage, data+pos);
    pos += write_short(slot->nbt_len, data+pos);

    if (slot->nbt_len != -1){
	memcpy(data+pos, slot->nbt, slot->nbt_len);
	pos += slot->nbt_len;
    }
    *len = pos;
    return data;
}
	
size_t Slot_encoded_size(Slot *slot){
    size_t nbtlen = slot->nbt_len == -1 ? 0 : slot->nbt_len;
    return SIZE_SHORT + SIZE_BYTE + SIZE_SHORT + SIZE_SHORT + nbtlen;
}

void Slot_free(Slot *slot){
    if (slot->nbt_len != -1) free(slot->nbt);
    free(slot);
}

char Slot_is_empty(Slot *slot){
    if (slot->id == -1) return 1;
    if (slot->count <= 0) return 1;
    return 0;
}

Slot * Slot_new_empty(){
    Slot *slot = malloc(sizeof(Slot));
    slot->id = -1;
    slot->count = 0;
    slot->damage = 0;
    slot->nbt_len = -1;
    slot->nbt = NULL;
    return slot;
}

Slot * Slot_new_basic(short id, char count, short damage){
    Slot *slot = malloc(sizeof(Slot));
    slot->id = id;
    slot->count = count;
    slot->damage = damage;
    slot->nbt_len = -1;
    slot->nbt = NULL;
    return slot;
}

Slot * Slot_copy(Slot *slot){
    logmsg(LOG_DEBUG, "Copying a slot.");
    Slot *newslot = malloc(sizeof(Slot));
    newslot->id = slot->id;
    newslot->count = slot->count;
    newslot->damage = slot->damage;
    newslot->nbt_len = slot->nbt_len;
    if (newslot->nbt_len != -1){
	logmsg(LOG_DEBUG, "Copying NBT Data!");
	char *newnbt = malloc(sizeof(char) * newslot->nbt_len);
	memcpy(newnbt, slot->nbt, newslot->nbt_len);
	newslot->nbt = newnbt;
    }
    logmsg(LOG_DEBUG, "Done copying slot.");
    return newslot;
}
	
int Slot_can_add(Slot *slot, Slot *other){
    if (slot->id != other->id) return 0;
    if (slot->damage != other->damage) return 0;
    if (slot->count + other->count > 64) return 0;
    return 1;
}

void Slot_add(Slot *slot, Slot *other){
    if (slot == NULL || other == NULL){
	logmsg(LOG_WARN, "Slot_add should not be used with NULL slots!");
	return;
    }
    if (slot->id != other->id) return;
    if (slot->damage != other->damage) return;
    int max_added = 64 - slot->count;
    if (other->count <= max_added){
	slot->count += other->count;
	other->count = 0;
    } else {
	slot->count += max_added;
	other->count -= max_added;
    }
    
}

Inventory * Inventory_new_empty(size_t size){
    Inventory *i = malloc(sizeof(Inventory));
    i->size = size;
    i->slots = malloc(sizeof(Slot*) * i->size);

    int j;
    for (j=0; j < i->size; j++){
	i->slots[j] = Slot_new_empty();
    }
    return i;
}

void Inventory_free(Inventory *inv){
    for (int i=0; i < inv->size; i++){
	Slot *s = Inventory_get(inv, i);
	if (s != NULL){
	    Slot_free(s);
	}
    }
    free(inv->slots);
    free(inv);
}
void Inventory_set(Inventory *inv, Slot *slot, size_t index){
    if (index > inv->size - 1){
	logmsg(LOG_WARN, "Tried to set invalid slot index!");
	return;
    }
    Slot *old = Inventory_get(inv, index);
    if (old != NULL) Slot_free(old);

    inv->slots[index] = slot;
}
Slot *Inventory_get(Inventory *inv, size_t index){
    if (index >= inv->size || index < 0) {
	logmsg(LOG_WARN, "Tried to get inventory item of invalid index!");
	return NULL;
    }
    return inv->slots[index];
}
int Inventory_player_add_item(Inventory *inv, Slot *slot){
    // Check the hotbar.
    int i;
    for (i=36; i < 45; i++){
	if (Slot_is_empty(inv->slots[i])){
	    Inventory_set(inv, Slot_copy(slot), i);
	    slot->count = 0;
	    slot->id = -1;
	    return 1;
	}
        Slot_add(Inventory_get(inv, i), slot);
	if (Slot_is_empty(slot)) return 1;
    }
    return 0;
    
}
    
