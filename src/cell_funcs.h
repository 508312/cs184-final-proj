#ifndef CELL_FUNCS_H
#define CELL_FUNCS_H

//#include <CGL/vector3D.h>
#include "intvec3.h"

inline uint8_t getRandom(uint8_t from, uint8_t to) {
	return rand()%(to-from) + from;
};

#define SAND_COLOR {getRandom(220, 255), getRandom(200, 255), getRandom(0, 10), 255}

class Chunk;

extern void updateError(Chunk* chunk, vec3 pos);
extern void updateAir(Chunk* chunk, vec3 pos);
extern void updateSand(Chunk* chunk, vec3 pos);
extern void updateWater(Chunk* chunk, vec3 pos);

#endif