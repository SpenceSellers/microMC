#ifndef INVENTORY_H
#define INVENTORY_H
#include <stdlib.h>
typedef struct Slot {
    short id;
    char count;
    short damage;
    short nbt_len;
    char *nbt;
} Slot;

Slot * Slot_read(char *data, size_t *read);
char * Slot_encode(Slot *slot, size_t *len);
void Slot_free(Slot *slot);

char Slot_is_empty(Slot *slot);
Slot * Slot_new_empty();
Slot * Slot_new_basic(short id, char count, short damage);

int Slot_can_add(Slot *slot, Slot *other);

typedef struct Inventory {
    size_t size;
    Slot **slots;
} Inventory;

Inventory * Inventory_new_empty(size_t size);

/* This WILL free all of the slots associated with the inventory.
 * You should remove or duplicate anything which you wish to keep
 * before freeing the inventory. */
void Inventory_free(Inventory *i);
size_t Slot_encoded_size(Slot *slot);

/* Sets a slot in the inventory.
 *  This WILL free whatever was there before.*/
void Inventory_set(Inventory *inv, Slot *slot, size_t index);
Slot *Inventory_get(Inventory *inv, size_t index);
int Inventory_player_add_item(Inventory *inv, Slot *slot);
#endif
