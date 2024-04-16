#include <iostream>
#include <CGL/vector3D.h>
#include <random>

#include "cell_funcs.h"
#include "chunk.h"

void updateError(void* chunk, CGL::Vector3D pos) {
	Chunk* pChunk = (Chunk*)chunk;
	std::cout << "UPDATING ELEMENT WHICH YOU SHOULD NOT " << std::endl;
	return;
}

void updateAir(void* chunk, CGL::Vector3D pos) {
	Chunk* pChunk = (Chunk*)chunk;
	return;
}

inline CGL::Vector3D get_random(int& size, CGL::Vector3D* arr) {
	int ind = rand() % size;
	CGL::Vector3D ret = arr[ind];
	size -= 1;
	arr[ind] = arr[size];
	return ret;
}

void updateSand(void* chunk, CGL::Vector3D curr_pos) {
	CGL::Vector3D dirs[] = { CGL::Vector3D(-1, -1, -1),
							 CGL::Vector3D(0, -1, -1),
							 CGL::Vector3D(1, -1, -1),
							 CGL::Vector3D(1, -1, 0),
							 CGL::Vector3D(1, -1, 1),
							 CGL::Vector3D(0, -1, 1),
							 CGL::Vector3D(-1, -1, 1),
							 CGL::Vector3D(-1, -1, 0) };

	Chunk* pChunk = (Chunk*)chunk;

	if (pChunk->getCell(curr_pos + CGL::Vector3D(0, -1, 0)).type == AIR) {
		pChunk->swapCells(curr_pos, curr_pos + CGL::Vector3D(0, -1, 0));
	}
	else {
		int size = 8;
		while (size != 0) {
			CGL::Vector3D new_pos = curr_pos + get_random(size, dirs);
			if (pChunk->getCell(new_pos).type == AIR) {
				pChunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	}
}
