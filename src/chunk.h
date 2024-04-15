#ifndef CHUNK_H
#define CHUNK_H

#include "cell_types.h"
#include "cell.h"
#include <bitset>
#include <array>

// Size of a side of a chunk. Should be a power of 2.
#define CHUNK_SIZE 16

/** Manages space in this CHUNK_SIZE^3 world space. **/
class Chunk {
	public:
		Chunk();
		void update();
		cell& getCell(int x, int y, int z);
		void setCell(int x, int y, int z, cell& cell);
		void swapCells(int x, int y, int z, int xto, int yto, int zto);

	private:
		std::array<cell, CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE> cells;

		// keep track of modified cells to no update twice per tick.
		std::bitset<CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE> dirty_cells;
};


#endif // CHUNK_H