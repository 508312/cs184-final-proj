#ifndef CHUNK_H
#define CHUNK_H

#include "cell.h"
#include <bitset>
#include <array>

// Size of a side of a chunk. Should be a power of 2.
#define CHUNK_SIZE 32

class World;

/** Manages space in this CHUNK_SIZE^3 world space. **/
class Chunk {
	public:
		Chunk(World* wp, vec3 chunk_index);
		void update();
		cell& getCell(vec3 pos);
		void setCell(vec3 pos, cell cell);
		void spawnCell(vec3 pos, cell cell);
		void swapCells(vec3 curr_pos, vec3 new_pos);
		void simulate(int simulation_steps);

		int getIndex(vec3 pos) { return pos[0] + pos[1] * CHUNK_SIZE + pos[2] * CHUNK_SIZE * CHUNK_SIZE; }

		bool needsUpdate() { return !(bbox_to == vec3(-1, -1, -1)); }

		vec3 getChunkPos() { return chunk_pos; }

		void expandBbox(vec3& pos);

		vec3 getBboxFrom() { return bbox_from; }
		vec3 getBboxTo() { return bbox_to; }

		void resetDirty();
		void dumpChunk(std::ofstream& file);
		void loadChunk(std::ifstream& file);

	private:
		void resetBbox();

		std::array<cell, CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE> cells;

		// keep track of modified cells to no update twice per tick.
		std::bitset<CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE> dirty_cells;

		vec3 bbox_from;
		vec3 bbox_to;

		World* world;
		vec3 chunk_pos;
};


#endif // CHUNK_H