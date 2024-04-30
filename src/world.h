#ifndef WORLD_H
#define WORLD_H

#include "cell.h"
#include <unordered_map>
#include "intvec3.h"
#include "chunk.h"
#include <math.h>
#include <vector>

/** Manages space in this CHUNK_SIZE^3 world space. **/

#define WORLD_SIZE 20000

// mod which works with negative numbers
inline int umod(int a, int b) {
	int r = a % b;
	return r < 0 ? r + b : r;
}

class World {
	public:
		World();
		~World();
		std::vector<Chunk*> update();
		cell& getCell(vec3 pos);
		void setCell(vec3 pos, cell cell);
		void spawnCell(vec3 pos, cell cell);
		void swapCells(vec3 curr_pos, vec3 new_pos);

		void createChunkIfDoesntExist(vec3 idx);

		vec3 getPosInChunk(vec3 pos) { return vec3(umod(pos[0], CHUNK_SIZE), umod(pos[1], CHUNK_SIZE), umod(pos[2], CHUNK_SIZE)); }

		// block pos to chunk
		int getChunkIndex(vec3 pos) {
			// todo give name to const
			pos = getChunkPos(pos);
			return pos[0] + pos[1] * WORLD_SIZE + pos[2] * WORLD_SIZE * WORLD_SIZE;
		}

		vec3 getChunkPos(vec3 pos) { return vec3(std::floor(pos[0] / (float)CHUNK_SIZE),
												std::floor(pos[1] / (float)CHUNK_SIZE),
												std::floor(pos[2] / (float)CHUNK_SIZE)); }


		
		std::vector<Chunk*> getChunks();

		// block pos
		Chunk* getChunkAtBlock(vec3 pos) { return getChunk(getChunkIndex(pos)); }

		void dumpWorld(std::string filename);
		void loadWorld(std::string filename);

	private:
		Chunk* getChunk(int idx) { return chunks[idx]; }

		std::unordered_map<int, Chunk*> chunks;
		std::vector<Chunk*> newly_created;
};


#endif // WORLD_H