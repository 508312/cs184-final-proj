#include <iostream>
#include <CGL/vector3D.h>
#include <random>

#include "cell_funcs.h"
#include "chunk.h"

void updateError(Chunk* chunk, CGL::Vector3D pos) {
	std::cout << "UPDATING ELEMENT WHICH YOU SHOULD NOT " << std::endl;
	return;
}

void updateAir(Chunk* chunk, CGL::Vector3D pos) {
	return;
}

inline CGL::Vector3D get_random(int& size, CGL::Vector3D* arr) {
	int ind = rand() % size;
	CGL::Vector3D ret = arr[ind];
	size -= 1;
	arr[ind] = arr[size];
	return ret;
}

void updateSand(Chunk* chunk, CGL::Vector3D curr_pos) {
	CGL::Vector3D dirs[] = { CGL::Vector3D(-1, -1, -1),
							 CGL::Vector3D(0, -1, -1),
							 CGL::Vector3D(1, -1, -1),
							 CGL::Vector3D(1, -1, 0),
							 CGL::Vector3D(1, -1, 1),
							 CGL::Vector3D(0, -1, 1),
							 CGL::Vector3D(-1, -1, 1),
							 CGL::Vector3D(-1, -1, 0) };

	if (chunk->getCell(curr_pos + CGL::Vector3D(0, -1, 0)).type == AIR) {
		chunk->swapCells(curr_pos, curr_pos + CGL::Vector3D(0, -1, 0));
	}
	else {
		int size = 8;
		while (size != 0) {
			CGL::Vector3D new_pos = curr_pos + get_random(size, dirs);
			if (chunk->getCell(new_pos).type == AIR) {
				chunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	}
}
void updateWater(Chunk* chunk, CGL::Vector3D curr_pos) {
	CGL::Vector3D dirs[] = { CGL::Vector3D(-1, -1, -1),
							 CGL::Vector3D(0, -1, -1),
							 CGL::Vector3D(1, -1, -1),
							 CGL::Vector3D(1, -1, 0),
							 CGL::Vector3D(1, -1, 1),
							 CGL::Vector3D(0, -1, 1),
							 CGL::Vector3D(-1, -1, 1),
							 CGL::Vector3D(-1, -1, 0),
							 CGL::Vector3D(-1, 0, -1),
							 CGL::Vector3D(0, 0, -1),
							 CGL::Vector3D(1, 0, -1),
							 CGL::Vector3D(1, 0, 0),
							 CGL::Vector3D(1, 0, 1),
							 CGL::Vector3D(0, 0, 1),
							 CGL::Vector3D(-1, 0, 1),
							 CGL::Vector3D(-1, 0, 0),
							};

	if (chunk->getCell(curr_pos + CGL::Vector3D(0, -1, 0)).type == AIR) {
		chunk->swapCells(curr_pos, curr_pos + CGL::Vector3D(0, -1, 0));
	}
	else {
		int size = 16;
		while (size != 0) {
			CGL::Vector3D new_pos = curr_pos + get_random(size, dirs);
			if (chunk->getCell(new_pos).type == AIR) {
				chunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	}
}
