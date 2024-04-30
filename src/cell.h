#ifndef CELL_H
#define CELL_H

#include <stdint.h>
#include "cell_types.h"
#include "properties.h"
#include <fstream>
#include <istream>
#include <iostream>

struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct cell {
	color color;
	CellType type;
	// arbitrary attribute if we need it. For example duration left on flame(it starts with 10, and ticks down every tick, upon reaching 0 it extinguishes)
	//attrubute attr,
};


typedef union {
	color color;
	uint32_t bits;
} pushable_color;

inline std::ostream& operator<<(std::ostream& file, color& color) {
	pushable_color icolor;
	icolor.color = color;
	file << icolor.bits;
	return file;
};

inline std::istream& operator>>(std::istream& file, color& color) {
	pushable_color icolor;
	file >> icolor.bits;
	color = icolor.color;
	return file;
};

inline std::ostream& operator<<(std::ostream& file, cell& cell) {
	file << cell.color << " " << cell.type;
	return file;
};

inline std::istream& operator>>(std::istream& file, cell& cell) {
	int inttype;
	file >> cell.color >> inttype;
	cell.type = (CellType)inttype;
	return file;
};

#endif