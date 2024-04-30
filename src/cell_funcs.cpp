#include <iostream>
#include <vector>
//#include <CGL/vector3D.h>
#include <random>
#include "intvec3.h"
#include "cell_funcs.h"
#include "chunk.h"

bool isAbovePercentage(float percentage) {
	return (float)rand() / RAND_MAX > percentage;
}

void updateError(Chunk* chunk, vec3 pos) {
	//std::cout << "UPDATING ELEMENT WHICH YOU SHOULD NOT " << std::endl;
	return;
}

void updateAir(Chunk* chunk, vec3 pos) {
	return;
}

inline vec3 get_random(int& size, vec3* arr) {
	int ind = rand() % size;
	vec3 ret = arr[ind];
	size -= 1;
	arr[ind] = arr[size];
	return ret;
}

void updateSand(Chunk* chunk, vec3 curr_pos) {
	vec3 dirs[] = { vec3(-1, -1, -1),
							 vec3(0, -1, -1),
							 vec3(1, -1, -1),
							 vec3(1, -1, 0),
							 vec3(1, -1, 1),
							 vec3(0, -1, 1),
							 vec3(-1, -1, 1),
							 vec3(-1, -1, 0) };
	if (chunk->getCell(curr_pos + vec3(0, -1, 0)).type == AIR) {
		chunk->swapCells(curr_pos, curr_pos + vec3(0, -1, 0));
	}
	else {
		int size = 8;
		while (size != 0) {
			vec3 new_pos = curr_pos + get_random(size, dirs);
			if (chunk->getCell(new_pos).type == AIR) {
				chunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	}
}
void updateWater(Chunk* chunk, vec3 curr_pos) {
	vec3 dirs[] = { vec3(-1, -1, -1),
							 vec3(0, -1, -1),
							 vec3(1, -1, -1),
							 vec3(1, -1, 0),
							 vec3(1, -1, 1),
							 vec3(0, -1, 1),
							 vec3(-1, -1, 1),
							 vec3(-1, -1, 0),
							 vec3(-1, 0, -1),
							 vec3(0, 0, -1),
							 vec3(1, 0, -1),
							 vec3(1, 0, 0),
							 vec3(1, 0, 1),
							 vec3(0, 0, 1),
							 vec3(-1, 0, 1),
							 vec3(-1, 0, 0),
							};

	if (chunk->getCell(curr_pos + vec3(0, -1, 0)).type == AIR) {
		chunk->swapCells(curr_pos, curr_pos + vec3(0, -1, 0));
	}
	else {
		int size = 16;
		while (size != 0) {
			vec3 new_pos = curr_pos + get_random(size, dirs);
			if (chunk->getCell(new_pos).type == AIR) {
				chunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	}
}

void updateFire(Chunk* chunk, vec3 curr_pos) {
	vec3 dirs[] = { vec3(-1, -1, -1),
							 vec3(0, -1, 0),
							 vec3(0, -1, -1),
							 vec3(1, -1, -1),
							 vec3(1, -1, 0),
							 vec3(1, -1, 1),
							 vec3(0, -1, 1),
							 vec3(-1, -1, 1),
							 vec3(-1, -1, 0),
							 vec3(-1, 0, -1),
							 vec3(0, 0, -1),
							 vec3(1, 0, -1),
							 vec3(1, 0, 0),
							 vec3(1, 0, 1),
							 vec3(0, 0, 1),
							 vec3(-1, 0, 1),
							 vec3(-1, 0, 0),
							 vec3(0, 1, 0),
							 vec3(-1, 1, -1),
							 vec3(0, 1, -1),
							 vec3(1, 1, -1),
							 vec3(1, 1, 0),
							 vec3(1, 1, 1),
							 vec3(0, 1, 1),
							 vec3(-1, 1, 1),
							 vec3(-1, 1, 0),
	};
	// dissipate with a chance
	if (isAbovePercentage(0.95)) {
		chunk->setCell(curr_pos, cell{ AIR_COLOR, AIR });
		return;
	}

	std::vector<vec3> burnable;
	for (int i = 0; i < sizeof(dirs) / sizeof(vec3); i++) {
		if (chunk->getCell(curr_pos + dirs[i]).type == WATER) { 
			// if any of the adjacent cells are water, burn out immediately
			chunk->setCell(curr_pos + dirs[i], cell{ STEAM_COLOR, STEAM });
			chunk->setCell(curr_pos, cell{ STEAM_COLOR, STEAM });
			return;
		}
		else if (type2prop[chunk->getCell(curr_pos + dirs[i]).type]&PROPERTY_BURNABLE) {
			burnable.push_back(dirs[i]);
		}
	}

	// if the block directly above is empty, chance to emit smoke
	if (chunk->getCell(curr_pos + vec3(0, 1, 0)).type == AIR && isAbovePercentage(0.9)) {
		chunk->setCell(curr_pos + vec3(0, 1, 0), cell{ SMOKE_COLOR, SMOKE });
	}

	// if there is anything to burn in adjacent cells, chance to burn and spread
	// otherwise, chance to burn out
	if (burnable.size() == 0 && isAbovePercentage(0.9)) {
		chunk->setCell(curr_pos, cell{ AIR_COLOR, AIR });
	}
	else {
		for (int i = 0; i < burnable.size(); i++) {
			if (isAbovePercentage(0.7)) {
				chunk->setCell(curr_pos + burnable[i], cell{ FIRE_COLOR, FIRE });
			}
		}
		if (chunk->getCell(curr_pos + vec3(0, -1, 0)).type == AIR) {
			chunk->swapCells(curr_pos, curr_pos + vec3(0, -1, 0));
		}
	}
}

void updateSteam(Chunk* chunk, vec3 curr_pos) {
	vec3 dirs[] = {vec3(-1, 1, -1),
				   vec3(0, 1, -1),
				   vec3(1, 1, -1),
				   vec3(1, 1, 0),
				   vec3(1, 1, 1),
				   vec3(0, 1, 1),
				   vec3(-1, 1, 1),
				   vec3(-1, 1, 0),
	};
	// dissipate with a chance
	if (isAbovePercentage(0.7)) {
		chunk->setCell(curr_pos, cell{ AIR_COLOR, AIR });
		return;
	}
	if (chunk->getCell(curr_pos + vec3(0, 1, 0)).type == AIR && isAbovePercentage(0.4)) {
		chunk->swapCells(curr_pos, curr_pos + vec3(0, 1, 0));
	}
	else {
		int size = 8;
		while (size != 0) {
			vec3 new_pos = curr_pos + get_random(size, dirs);
			if (chunk->getCell(new_pos).type == AIR) {
				chunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	}
}

void updateSmoke(Chunk* chunk, vec3 curr_pos) {
	vec3 dirs[] = { vec3(-1, 1, -1),
				   vec3(0, 1, -1),
				   vec3(1, 1, -1),
				   vec3(1, 1, 0),
				   vec3(1, 1, 1),
				   vec3(0, 1, 1),
				   vec3(-1, 1, 1),
				   vec3(-1, 1, 0),
	};
	// dissipate with a chance
	if (isAbovePercentage(0.7)) {
		chunk->setCell(curr_pos, cell{ AIR_COLOR, AIR });
		return;
	}
	if (chunk->getCell(curr_pos + vec3(0, 1, 0)).type == AIR && isAbovePercentage(0.4)) {
		chunk->swapCells(curr_pos, curr_pos + vec3(0, 1, 0));
	}
	else {
		int size = 8;
		while (size != 0) {
			vec3 new_pos = curr_pos + get_random(size, dirs);
			if (chunk->getCell(new_pos).type == AIR) {
				chunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	}
}