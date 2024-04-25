#ifndef CELL_FUNCS_H
#define CELL_FUNCS_H

//#include <CGL/vector3D.h>
#include "intvec3.h"

class Chunk;

extern void updateError(Chunk* chunk, vec3 pos);
extern void updateAir(Chunk* chunk, vec3 pos);
extern void updateSand(Chunk* chunk, vec3 pos);
extern void updateWater(Chunk* chunk, vec3 pos);

#endif