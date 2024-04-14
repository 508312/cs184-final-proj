#ifndef CHUNK_H
#define CHUNK_H

#inlude "cell_types.h"
#inlude "cell.h"

// Size of a side of a chunk. Should be a power of 2.
#define CHUNK_SIZE 16

cell oob_cell = cell{ color{0, 0, 0, 0}, WALL, 0 };

/** Manages space in this CHUNK_SIZE^3 world space. **/
class Chunk {
	public:
		Chunk();
		void update();
		Cell& getCell(int x, int y, int z);
		void setCell(int x, int y, int z, Cell& cell);
		void moveCell(int x, int y, int z, Cell& cell);

	private:
		std::array<cell, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> cells;

		// keep track of modified cells to no update twice per tick.
		std::bitset<CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> dirty_cells;
}


#endif // CHUNK_H