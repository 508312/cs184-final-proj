#ifndef CELL_FUNCS_H
#define CELL_FUNCS_H

#include "chunk.h"

updateError(Chunk* chunk, int x, int y, int z);
updateAir(Chunk* chunk, int x, int y, int z);
updateSand(Chunk* chunk, int x, int y, int z);

#endif