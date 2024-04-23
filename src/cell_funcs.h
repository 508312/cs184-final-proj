#ifndef CELL_FUNCS_H
#define CELL_FUNCS_H

#include <CGL/vector3D.h>

class Chunk;

extern void updateError(Chunk* chunk, CGL::Vector3D pos);
extern void updateAir(Chunk* chunk, CGL::Vector3D pos);
extern void updateSand(Chunk* chunk, CGL::Vector3D pos);
extern void updateWater(Chunk* chunk, CGL::Vector3D pos);

#endif