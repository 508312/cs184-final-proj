# include "chunk.h"
# include "cell_types.h"
# include "cell.h"
# include <cassert>

Chunk::Chunk() {
    dirty_cells.reset();
}

void Chunk::update() {
    // process 1 tick of time
    for (int x = 1, x < CHUNK_SIZE - 1; x++) {
        for (int y = 1; y < CHUNK_SIZE - 1; y++) {
            for (int z = 1; z < CHUNK_SIZE - 1; z++) {
                type2Func[getCell(x, y, z).cellType](this, x, y, z);
            }
        }
    }
}

cell& getCell(int x, int y, int z) {
    double size = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;

    if (size > CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) {
        return oob_cell;
    }

    return cells[size];
}

void setCell(int x, int y, int z, Cell& cell) {
    getCell(x,y,z) = cell;
}
void moveCell(int x, int y, int z, Cell& cell) {
    
}