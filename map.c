#include "map.h"
#include "logging.h"
#include <stdio.h>
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

Block Chunk_get_block(Chunk *chunk, int x, int y, int z){
    size_t blockpos = y * 256 + z * 16 + x;
    return chunk->blocks[blockpos];
}

void Chunk_set_below(Chunk *chunk, Block b, int y){
    int x;
    int z;
    int level;
    for (x=0; x < 16; x++){
	for (z=0; z<16; z++){
	    for (level=0; level<y; level++){
		Chunk_set_block(chunk, x, level, z, b);
	    }
	}
    }
}
// Checks to see if a chunk is within the map's area.
char Map_validate_chunk_coords(Map *map, ssize_t x, ssize_t z){
    if (x > map->xchunks-1 || z > map->zchunks-1 || x < 0 || z < 0){
        return 0;
    }
    return 1;
}

Map * Map_new_empty(ssize_t x, ssize_t z){
    logmsg(LOG_DEBUG, "Generating new empty map.");
    Map *map = malloc(sizeof(Map));
    map->chunks = malloc(sizeof(void *) * x * z);
    map->xchunks = x;
    map->zchunks = z;

    int i;
    for (i=0; i < x * z; i++){
	map->chunks[i] = NULL;
    }
    return map;
}

Map * Map_new_air(ssize_t x, ssize_t z){
    Map *map = Map_new_empty(x, z);
    int i;
    int j;
    for (i=0; i < x; i++){
	for (j=0; j < z; j++){
	    Chunk *c = Chunk_new_empty();
	    Map_set_chunk(map, c, i, j);
	}
    }
    return map;
}
Chunk * Map_get_chunk(Map *map, ssize_t x, ssize_t z){
    if (!Map_validate_chunk_coords(map, x, z)){
	logmsg(LOG_DEBUG, "Tried to get invalid chunk!");
	return NULL;
    }

    return map->chunks[(x * map->xchunks) + z];
}

void Map_set_chunk(Map *map, Chunk *chunk, ssize_t x, ssize_t z){
    if (!Map_validate_chunk_coords(map, x, z)){
	logmsg(LOG_WARN, "Tried to set an invalid chunk position!");
	return;
    }
    map->chunks[(x * map->xchunks) + z] = chunk;
}

void Map_set_block(Map *map, Block block, int x, int y, int z){
    ssize_t chunkx = x / 16;
    ssize_t chunkz = z / 16;

    Chunk *chunk = Map_get_chunk(map, chunkx, chunkz);
    if (chunk == NULL){
	logmsg(LOG_WARN, "Tried to set block that is outside of map!");
	return;
    }
    Chunk_set_block(chunk, x%16, y, z%16, block);
}
Block Map_get_block(Map *map, int x, int y, int z){
    ssize_t chunkx = x / 16;
    ssize_t chunkz = z / 16;

    Chunk *chunk = Map_get_chunk(map, chunkx, chunkz);
    if (chunk == NULL){
	logmsg(LOG_WARN, "Tried to get block that is outside of map!");
	return (Block) {.id = 0, .metadata = 0};
    }
    return Chunk_get_block(chunk, x%16, y, z%16);
}
void Map_set_below(Map *map, Block b, int level){
    int x = map->xchunks;
    int z = map->zchunks;
    int i;
    int j;
    for (i=0; i < x; i++){
	for (j=0; j < z; j++){
	    Chunk_set_below(Map_get_chunk(map, i, j), b, level);
	}
    }
}

void Map_write(Map *map, char *fname){
    logfmt(LOG_INFO, "Writing map: %s", fname);
    FILE *f = fopen(fname, "wb");
    int xsize = map->xchunks;
    int zsize = map->zchunks;
    
    fwrite(&xsize, sizeof(int), 1, f);
    fwrite(&zsize, sizeof(int), 1, f);

    for (int xchunk = 0; xchunk < xsize; xchunk++){
	for (int zchunk = 0; zchunk < zsize; zchunk++){
	    Chunk *chunk = Map_get_chunk(map, xchunk, zchunk);
	    for (int blockpos = 0; blockpos < 16*16*256; blockpos++){
		Block b = chunk->blocks[blockpos];
		char id = b.id;
		char meta = b.metadata;

		fwrite(&id, sizeof(char), 1, f);
		fwrite(&meta, sizeof(char), 1, f);
	    }
	}
    }
    fclose(f);
}
Map * Map_read(char *fname){
    logfmt(LOG_INFO, "Reading map: %s", fname);
    FILE *f = fopen(fname, "rb");
    int xsize;
    int zsize;

    fread(&xsize, sizeof(int), 1, f);
    fread(&zsize, sizeof(int), 1, f);
    logfmt(LOG_DEBUG, "Map is of size %d %d", xsize, zsize); 
    Map *map = Map_new_empty(xsize, zsize);

    for (int xchunk = 0; xchunk < xsize; xchunk++){
	for (int zchunk = 0; zchunk < zsize; zchunk++){
	    
	    Chunk *chunk = Chunk_new_empty();
	    Map_set_chunk(map, chunk, xchunk, zchunk);
	    for (int blockpos = 0; blockpos < 16*16*256; blockpos++){
		
		char id;
		char meta;
		fread(&id, sizeof(char), 1, f);
		fread(&meta, sizeof(char), 1, f);
		Block b = {.id = id, .metadata = meta};
		chunk->blocks[blockpos] = b;
	    }
	}
    }
    fclose(f);
    return map;
}

void apply_face(char face, int *x, int *y, int *z){
    switch (face){
    case 0:
	*y -= 1;
	break;
    case 1:
	*y += 1;
	break;
    case 2:
	*z -= 1;
	break;
    case 3:
	*z += 1;
	break;
    case 4:
	*x -= 1;
	break;
    case 5:
	*x += 1;
	break;
    }
}
