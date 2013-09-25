#include "map.h"
#include "logging.h"

#include <stdlib.h>
#define DEFAULT_BIOME 1

Chunk * Chunk_new_empty(){
    logmsg(LOG_DEBUG, "Making new empty chunk.");
    Chunk *chunk = malloc(sizeof(Chunk));
    Block airblock;
    airblock.id = 0;
    airblock.metadata = 0;

    int i;
    for (i=0; i < 16*16*256; i++){
	chunk->blocks[i] = airblock;
    }
    for(i=0; i < 16*16; i++){
	chunk->biome_data[i] = DEFAULT_BIOME;
    }
    
    logmsg(LOG_DEBUG, "Finishing making chunk.");
    return chunk;
}

void Chunk_set_block(Chunk *chunk, int x, int y, int z, Block block){
    size_t blockpos = y * 256 + z * 16 + x;

    chunk->blocks[blockpos] = block;
}
