#ifndef CELL_TYPES_H
#define CELL_TYPES_H

//#include "properties.h"
#include "cell_funcs.h"

#define CELLS \
CELL(WALL, PROPERTY_EMPTY, updateError) \
CELL(AIR, PROPERTY_EMPTY, updateAir) \
CELL(SAND, PROPERTY_EMPTY, updateSand)

#define CELL(m, p, f) m,
enum CellType {
	CELLS
};
#undef CELL

#define CELL(m, p, f) f,
constexpr static void (*type2func[]) (void*, CGL::Vector3D) = {
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