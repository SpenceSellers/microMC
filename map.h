#ifndef MAP_H
#define MAP_H
#include <stdlib.h>

typedef struct Block {
    unsigned char id;
    unsigned char metadata; // This is really a half-byte.
} Block;
    
typedef struct Chunk {
    Block blocks[16*16*256];
    char biome_data[16*16];
} Chunk;

Chunk * Chunk_new_empty();

void Chunk_set_block(Chunk *chunk, int x, int y, int z, Block block);
Block Chunk_get_block(Chunk *chunk, int x, int y, int z);
void Chunk_set_below(Chunk *chunk, Block b, int y);
Chunk * Chunk_new_empty();

typedef struct Map {
    size_t xchunks;
    size_t zchunks;
    Chunk **chunks;

} Map;

// Generates a new map with null chunks.
Map * Map_new_empty(ssize_t x, ssize_t z);

Map * Map_new_air(ssize_t x, ssize_t z);

/* Returns the chunk designated by a given chunk coords.
 * This is in CHUNK COORDINATES, not block coordinates.
 */
Chunk * Map_get_chunk(Map *map, ssize_t x, ssize_t z);
/*
 * Sets a chunk using chunk coordinates. Does not free the existing
 * chunk, if one existed.
 */
void Map_set_chunk(Map *map, Chunk *chunk, ssize_t x, ssize_t z);

void Map_set_block(Map *map, Block block, int x, int y, int z);

/* Applies the "face" byte onto position coordinates.
 * These WILL probably need to be checked for santiy afterwards.
 */
void apply_face(char face, int *x, int *y, int *z);
#endif
