#ifndef CELL_TYPES_H
#define CELL_TYPES_H

#include "properties.h"
#include "cell_funcs.h"

// Usage: to add new cell type add MAT(NAME, PROPERTIES, SIM_FUNC)
#define CELLS \
CELL(WALL, PROPERTY_EMPTY, updateError) \
CELL(AIR, PROPERTY_EMPTY, updateAir) \
CELL(SAND, PROPERTY_EMPTY, updateSand) \




#define CELL(m, p, f) m,
enum CellTypes {
	CELLS		   
};
#undef CELL

#define CELL(m, p, f) p,
auto type2prop = {
	CELLS
};
#undef CELL

#define CELL(m, p, f) f,
auto type2func = {
	CELLS
};
#undef CELL



#endif