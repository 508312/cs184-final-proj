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
			// if any of the adjacent cells are water, burn out both water and fire block
			chunk->setCell(curr_pos + dirs[i], cell{ STEAM_COLOR, STEAM });
			chunk->setCell(curr_pos, cell{ STEAM_COLOR, STEAM });
			return;
		}
		if (chunk->getCell(curr_pos + dirs[i]).type == SNOW) {
			// if any of the adjacent cells are snow, burn out fire and melt snow
			chunk->setCell(curr_pos + dirs[i], cell{ WATER_COLOR, WATER });
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
		if (isAbovePercentage(0.5)) {
			chunk->setCell(curr_pos, cell{ SMOKE_COLOR, SMOKE });
		} else {
			chunk->setCell(curr_pos, cell{ AIR_COLOR, AIR });
		}
	}
	else {
		for (int i = 0; i < burnable.size(); i++) {
			if (isAbovePercentage(0.85)) {
				chunk->setCell(curr_pos + burnable[i], cell{ FIRE_COLOR, FIRE });
			}
			else {
				chunk->setCell(curr_pos + burnable[i], cell{ SMOKE_COLOR, SMOKE });
			}
		}

		int size = 25;
		while (size != 0) {
			vec3 new_pos = curr_pos + get_random(size, dirs);
			if (chunk->getCell(new_pos).type == AIR) {
				chunk->swapCells(curr_pos, new_pos);
				return;
			}
		}

		chunk->setCell(curr_pos, chunk->getCell(curr_pos));
	}
}

void updateSnow(Chunk* chunk, vec3 curr_pos) {
	vec3 dirs[] = {
							 vec3(0, -1, -1),
							 vec3(1, -1, 0),
							 vec3(0, -1, 1),
							 vec3(-1, -1, 0) };
	if (chunk->getCell(curr_pos + vec3(0, -1, 0)).type == AIR) {
		chunk->swapCells(curr_pos, curr_pos + vec3(0, -1, 0));
	}
	else if (chunk->getCell(curr_pos + vec3(0, -1, 0)).type == SNOW) {
		int size = 4;
		while (size != 0) {
			vec3 new_pos = curr_pos + get_random(size, dirs);
			if (chunk->getCell(new_pos).type == AIR) {
				chunk->swapCells(curr_pos, new_pos);
				return;
			}
		}
	}
}

void updateGrass(Chunk* chunk, vec3 curr_pos) {
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
							 vec3(-1, 1, -1),
							 vec3(0, 1, -1),
							 vec3(1, 1, -1),
							 vec3(1, 1, 0),
							 vec3(1, 1, 1),
							 vec3(0, 1, 1),
							 vec3(-1, 1, 1),
							 vec3(-1, 1, 0),
	};

	if (type2prop[chunk->getCell(curr_pos + vec3(0, -1, 0)).type] & PROPERTY_PLANTABLE &&
		(chunk->getCell(curr_pos + vec3(0, 1, 0)).type == GRASS ||
		chunk->getCell(curr_pos + vec3(0, 1, 0)).type == TOPGRASS)) {

		bool surrounded_by_grass = true;
		for (int i = 0; i < 23; i++) {
			if (dirs[i].y != 0) {
				continue;
			}
			if (chunk->getCell(curr_pos + dirs[i]).type != GRASS &&
				chunk->getCell(curr_pos + dirs[i]).type != TOPGRASS) {
				surrounded_by_grass = false;
				break;
			}
		}
		if (surrounded_by_grass) {
			return;
		}

		// spread to nearby plantable blocks
		for (int i = 0; i < 23; i++) {
			if (isAbovePercentage(0.99)
				&& type2prop[chunk->getCell(curr_pos + dirs[i]).type] & PROPERTY_PLANTABLE
				&& chunk->getCell(curr_pos + dirs[i] + vec3(0, 1, 0)).type == AIR) {
				chunk->setCell(curr_pos + dirs[i] + vec3(0, 1, 0), cell{ GRASS_COLOR, GRASS });
			}
		}

		chunk->setCell(curr_pos, chunk->getCell(curr_pos));
	}

	// fall
	if (chunk->getCell(curr_pos + vec3(0, -1, 0)).type == AIR) {
		chunk->swapCells(curr_pos, curr_pos + vec3(0, -1, 0));
	}
	// otherwise check if landed on plantable material
	else if (type2prop[chunk->getCell(curr_pos + vec3(0, -1, 0)).type] & PROPERTY_PLANTABLE) {
		int grow_length = getRandom(1, 4);
		for (int i = 1; i < grow_length + 1; i++) {
			if (chunk->getCell(curr_pos + vec3(0, i, 0)).type == AIR) {
				chunk->setCell(curr_pos + vec3(0, i, 0), cell{ GRASS_COLOR, GRASS });
			}
			else {
				// something blocking growth, set growth_length to actual length
				grow_length = i;
				if (chunk->getCell(curr_pos + vec3(0, i, 0)).type == TOPGRASS ||
					chunk->getCell(curr_pos + vec3(0, i, 0)).type == GRASS)
					return;
				else
					break;
			}
		}
		// set top grass block to special TOPGRASS type, indicates it's the end of a grass blade
		// and prevents further growth at later ticks 
		if (grow_length == 1) {
			chunk->setCell(curr_pos + vec3(0, grow_length, 0), cell{ TOPGRASS_COLOR, TOPGRASS });
		} else if (isAbovePercentage(0.6)) {
			// flower
			chunk->setCell(curr_pos + vec3(0, grow_length, 0), cell{ TOPGRASSFLOWER_COLOR, TOPGRASS });
		} else {
			chunk->setCell(curr_pos + vec3(0, grow_length, 0), cell{ TOPGRASS_COLOR, TOPGRASS });
		}
	}
	// otherwise die
	else {
		if (chunk->getCell(curr_pos + vec3(0, -1, 0)).type == GRASS) {
			//std::cout << "not dying due to grass" << std::endl;
			return;
		}
		//std::cout << "dying due to not being on plantable" << std::endl;
		chunk->setCell(curr_pos, cell{ AIR_COLOR, AIR });
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

void doNothing(Chunk* chunk, vec3 curr_pos) {
	return;
}