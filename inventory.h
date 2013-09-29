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
void Slot_free(Slot *slot);

char Slot_is_empty(Slot *slot);

#endif
