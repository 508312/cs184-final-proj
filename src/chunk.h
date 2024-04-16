#ifndef CHUNK_H
#define CHUNK_H

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
		cell& getCell(CGL::Vector3D pos);
		void setCell(CGL::Vector3D pos, cell cell);
		void swapCells(CGL::Vector3D curr_pos, CGL::Vector3D new_pos);
		void simulate(int simulation_steps);

		int getIndex(CGL::Vector3D pos) { return pos[0] + pos[1] * CHUNK_SIZE + pos[2] * CHUNK_SIZE * CHUNK_SIZE; };

	private:
		std::array<cell, CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE> cells;

		// keep track of modified cells to no update twice per tick.
		std::bitset<CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE> dirty_cells;
};


#endif // CHUNK_H