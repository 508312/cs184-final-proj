#include "cell_funcs.h"
#include <iostream>


void updateError(void* chunk, int x, int y, int z) {
	std::cout << "UPDATING ELEMENT WHICH YOU SHOULD NOT " << std::endl;
	return;
}

void updateAir(void* chunk, int x, int y, int z) {
	return;
}

void updateSand(void* chunk, int x, int y, int z) {
	return;
}