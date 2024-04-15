# include "chunk.h"
# include "cell_types.h"
# include <iostream>
# include "cell.h"
# include <cassert>

color black = color{ 0, 0, 0, 0 };
cell oob_cell = cell{ black, WALL };

Chunk::Chunk(void) {
    dirty_cells.reset();
}


void Chunk::update() {
    // process 1 tick of time
    for (int x = 1; x < CHUNK_SIZE - 1; x++) {
        for (int y = 1; y < CHUNK_SIZE - 1; y++) {
            for (int z = 1; z < CHUNK_SIZE - 1; z++) {
                type2func[getCell(x, y, z).type](this, x, y, z);
            }
        }
    }
}

cell& Chunk::getCell(int x, int y, int z) {
    double size = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;

    if (size > CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) {
        return oob_cell;
    }

    return cells[size];
}

void Chunk::setCell(int x, int y, int z, cell& cell) {
    getCell(x,y,z) = cell;
}

void Chunk::swapCells(int x, int y, int z, int xto, int yto, int zto) {
    return;
}           