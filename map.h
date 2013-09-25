#ifndef MAP_H
#define MAP_H

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

Chunk * Chunk_new_empty();
typedef struct Map Map;

#endif
