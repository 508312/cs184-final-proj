#ifndef CELL_H
#define CELL_H


struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct cell {
	color clr,
	CellType type,
	// arbitrary attribute if we need it. For example duration left on flame(it starts with 10, and ticks down every tick, upon reaching 0 it extinguishes)
	attrib_t attribute,
};

#endif