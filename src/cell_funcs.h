#ifndef CELL_FUNCS_H
#define CELL_FUNCS_H

#include <CGL/vector3D.h>

extern void updateError(void* chunk, CGL::Vector3D pos);
extern void updateAir(void* chunk, CGL::Vector3D pos);
extern void updateSand(void* chunk, CGL::Vector3D pos);

#endif