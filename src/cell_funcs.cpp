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

inline int get_random(int size, CGL::Vector3D* arr) {
	int ind = rand() % size;
	CGL::Vector3D ret = arr[ind];
	size -= 1;
	arr[ind] = arr[size];
	return ind % size;
}

void updateSand(void* chunk, CGL::Vector3D curr_pos) {
	std::cout << "Updating" << std::endl;
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
		std::cout << "Directly down" << std::endl;
		pChunk->swapCells(curr_pos, curr_pos + CGL::Vector3D(0, -1, 0));
	}
	/*
	else {
		int size = 8;
		while (size != 0) {
			int adj_index = get_random(8, dirs);
			size -= 1;
			CGL::Vector3D new_pos = curr_pos + dirs[adj_index];
			if (pChunk->getCell(new_pos).type == AIR) {
				std::cout << "Adjacently down ";
				pChunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	} */
}
