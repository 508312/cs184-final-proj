#ifndef CELL_TYPES_H
#define CELL_TYPES_H

//#include "properties.h"
#include "cell_funcs.h"
#include "intvec3.h"
class Chunk;

#define CELLS \
CELL(WALL, PROPERTY_EMPTY, updateError) \
CELL(AIR, PROPERTY_EMPTY, updateAir) \
CELL(SAND, PROPERTY_EMPTY, updateSand) \
CELL(WATER, PROPERTY_EMPTY, updateWater) \
CELL(FIRE, PROPERTY_EMPTY, updateFire) \
CELL(STEAM, PROPERTY_EMPTY, updateSteam)

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

/*
#define CELL(m, p, f) p,
constexpr static property type2prop = {
	CELLS
};
#undef CELL
*/

// Usage: to add new cell type add MAT(NAME, PROPERTIES, SIM_FUNC)

#endif