# include "chunk.h"
# include "cell_types.h"
# include <iostream>
# include "cell.h"
# include <cassert>

color black = color{ 0, 0, 0, 0 };
cell oob_cell = cell{ black, WALL };

Chunk::Chunk(void) {
    dirty_cells.reset();

    // air
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                setCell(x, y, z, cell{ black, AIR });
            }
        }
    }

    // floor 
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            setCell(x, 0, z, cell{ black, SAND });
        }
    }

    setCell(3, 3, 3, cell{ black, SAND });
    setCell(6, 6, 6, cell{ black, SAND });
    setCell(1, 3, 1, cell{ black, SAND });
    setCell(1, 6, 1, cell{ black, SAND });
}

void Chunk::update() {
    std::cout << "TICKING " << std::endl;
    // process 1 tick of time
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (dirty_cells.test(getIndex(x, y, z)) == false) {
                    type2func[getCell(x, y, z).type](this, x, y, z);
                }
            }
        }
    }
    dirty_cells.reset();
}

cell& Chunk::getCell(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE ||
        y < 0 || y >= CHUNK_SIZE ||
        z < 0 || z >= CHUNK_SIZE) {
        return oob_cell;
    }

    return cells[getIndex(x, y, z)];
}

void Chunk::setCell(int x, int y, int z, cell cell) {
    cells[getIndex(x, y, z)] = cell;
    dirty_cells.set(getIndex(x, y, z));
    return;
}

void Chunk::swapCells(int x, int y, int z, int xto, int yto, int zto) {
    std::swap(getCell(x, y, z), getCell(xto, yto, zto));
    dirty_cells.set(getIndex(x, y, z));
    dirty_cells.set(getIndex(xto, yto, zto));
    return;
}           