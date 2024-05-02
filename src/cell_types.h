#ifndef CELL_TYPES_H
#define CELL_TYPES_H

#include "properties.h"
#include "cell_funcs.h"
#include "intvec3.h"
class Chunk;

#define CELLS \
CELL(WALL, PROPERTY_EMPTY, updateError) \
CELL(AIR, PROPERTY_EMPTY, updateAir) \
CELL(SAND, PROPERTY_BURNABLE | PROPERTY_PLANTABLE, updateSand) \
CELL(WATER, PROPERTY_EMPTY, updateWater) \
CELL(FIRE, PROPERTY_EMPTY, updateFire) \
CELL(SNOW, PROPERTY_EMPTY, updateSnow) \
CELL(GRASS, PROPERTY_EMPTY | PROPERTY_BURNABLE, updateGrass) \
CELL(TOPGRASS, PROPERTY_EMPTY | PROPERTY_BURNABLE, updateTopGrass) \
CELL(WOOD, PROPERTY_EMPTY | PROPERTY_BURNABLE | PROPERTY_PLANTABLE, doNothing) \
CELL(STEAM, PROPERTY_EMPTY, updateSteam) \
CELL(SMOKE, PROPERTY_EMPTY, updateSmoke) \
CELL(SAND_SPAWNER, PROPERTY_EMPTY, updateSandSpawner) \
CELL(WATER_SPAWNER, PROPERTY_EMPTY, updateWaterSpawner) \
CELL(FIRE_SPAWNER, PROPERTY_EMPTY, updateFireSpawner) \
CELL(SNOW_SPAWNER, PROPERTY_EMPTY, updateSnowSpawner) \
CELL(GRASS_SPAWNER, PROPERTY_EMPTY, updateGrassSpawner) \
CELL(WOOD_SPAWNER, PROPERTY_EMPTY, updateWoodSpawner) \
CELL(SMOKE_SPAWNER, PROPERTY_EMPTY, updateSmokeSpawner) \
CELL(STEAM_SPAWNER, PROPERTY_EMPTY, updateSteamSpawner) \

#define CELL(m, p, f) m,
enum CellType {
	CELLS
};
#undef CELL

#define CELL(m, p, f) f,
constexpr static void (*type2func[]) (Chunk*, vec3) = {
	CELLS
};
#undef CELL

#define CELL(m, p, f) p,
constexpr static property type2prop[] = {
	CELLS
};
#undef CELL

// Usage: to add new cell type add MAT(NAME, PROPERTIES, SIM_FUNC)

#endif