#include "cell_funcs.h"
#include <iostream>
#include "chunk.h"

void updateError(void* chunk, int x, int y, int z) {
	Chunk* pChunk = (Chunk*)chunk;
	std::cout << "UPDATING ELEMENT WHICH YOU SHOULD NOT " << std::endl;
	return;
}

void updateAir(void* chunk, int x, int y, int z) {
	Chunk* pChunk = (Chunk*)chunk;
	return;
}

void updateSand(void* chunk, int x, int y, int z) {
	Chunk* pChunk = (Chunk*)chunk;
	if (pChunk->getCell(x, y - 1, z).type == AIR) {
		pChunk->swapCells(x, y, z, x, y-1, z);
	}
}
