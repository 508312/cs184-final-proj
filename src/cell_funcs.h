#ifndef CELL_FUNCS_H
#define CELL_FUNCS_H

//#include <CGL/vector3D.h>
#include "intvec3.h"

inline uint8_t getRandom(uint8_t from, uint8_t to) {
	return rand()%(to-from) + from;
};

#define WALL_COLOR {0, 0, 0, 255}
#define AIR_COLOR {0, 0, 0, 0}
#define SAND_COLOR {getRandom(235, 255), getRandom(220, 255), getRandom(0, 10), 255}
#define WATER_COLOR {0, 0, 255, 50}
#define FIRE_COLOR {getRandom(235, 255), getRandom(0, 10), getRandom(0, 5), 255}
#define STEAM_COLOR {getRandom(120, 136), getRandom(120, 136), getRandom(120, 136), 50}
#define SMOKE_COLOR {getRandom(60, 80), getRandom(60, 80), getRandom(60, 80), 255}

#define GET_COLOR_FUNC(CLR) []() {return color CLR; }

class Chunk;

extern void updateError(Chunk* chunk, vec3 pos);
extern void updateAir(Chunk* chunk, vec3 pos);
extern void updateSand(Chunk* chunk, vec3 pos);
extern void updateWater(Chunk* chunk, vec3 pos);
extern void updateFire(Chunk* cunk, vec3 pos);
extern void updateSteam(Chunk* cunk, vec3 pos);
extern void updateSmoke(Chunk* cunk, vec3 pos);

#endif