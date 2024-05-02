#ifndef CELL_FUNCS_H
#define CELL_FUNCS_H

//#include <CGL/vector3D.h>
#include "intvec3.h"

inline uint8_t getRandom(uint8_t from, uint8_t to) {
	return rand()%(to-from) + from;
};

#define WALL_COLOR {getRandom(125, 145), getRandom(130, 150), getRandom(130, 150), 255}
#define AIR_COLOR {0, 0, 0, 0}
#define SAND_COLOR {getRandom(230, 245), getRandom(195, 220), getRandom(150, 175), 255}
#define WATER_COLOR {getRandom(0, 75), getRandom(100, 150), 255, 50}
#define FIRE_COLOR {getRandom(200, 255), getRandom(0, 90), getRandom(0, 5), 255}
#define SNOW_COLOR {getRandom(250, 255), getRandom(250, 255), getRandom(250, 255), 255}
#define GRASS_COLOR {getRandom(10, 50), getRandom(110, 150), getRandom(5, 20), 255}
#define TOPGRASS_COLOR {getRandom(10, 50), getRandom(110, 150), getRandom(5, 20), 255}
#define TOPGRASSFLOWER_COLOR {getRandom(60, 255), getRandom(20, 50), getRandom(80, 255), 255}
#define WOOD_COLOR {getRandom(70, 90), getRandom(50, 70), getRandom(30, 50), 255}
#define STEAM_COLOR {getRandom(120, 136), getRandom(120, 136), getRandom(120, 136), 50}
#define SMOKE_COLOR {getRandom(60, 80), getRandom(60, 80), getRandom(60, 80), 255}
#define SPAWNER_COLOR {getRandom(150, 180), getRandom(60, 80), getRandom(200, 255), 255}

#define GET_COLOR_FUNC(CLR) []() {return color CLR; }

class Chunk;

extern void updateError(Chunk* chunk, vec3 pos);
extern void updateAir(Chunk* chunk, vec3 pos);
extern void updateSand(Chunk* chunk, vec3 pos);
extern void updateWater(Chunk* chunk, vec3 pos);
extern void updateFire(Chunk* cunk, vec3 pos);
extern void updateSnow(Chunk* cunk, vec3 pos);
extern void updateGrass(Chunk* cunk, vec3 pos);
extern void updateSteam(Chunk* cunk, vec3 pos);
extern void updateSandSpawner(Chunk* chunk, vec3 pos);
extern void updateWaterSpawner(Chunk* chunk, vec3 pos);
extern void updateFireSpawner(Chunk* cunk, vec3 pos);
extern void updateSnowSpawner(Chunk* cunk, vec3 pos);
extern void updateGrassSpawner(Chunk* cunk, vec3 pos);
extern void updateWoodSpawner(Chunk* cunk, vec3 pos);
extern void updateSmokeSpawner(Chunk* cunk, vec3 pos);
extern void updateSteamSpawner(Chunk* cunk, vec3 pos);
extern void updateSmoke(Chunk* cunk, vec3 pos);
extern void updateTopGrass(Chunk* chunk, vec3 curr_pos);
extern void doNothing(Chunk* cunk, vec3 pos);

#endif