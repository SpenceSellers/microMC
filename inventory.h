#ifndef INVENTORY_H
#define INVENTORY_H
typedef struct Slot {
    short id;
    char count;
    short damage;
    short nbt_len;
    char *nbt;
} Slot;



#endif
