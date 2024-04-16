# include "chunk.h"
# include "cell_types.h"
# include <iostream>
# include "cell.h"
# include <cassert>

color black = color{ 0, 0, 0, 0 };
cell oob_cell = cell{ black, WALL };

Chunk::Chunk(void) {
    // air
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                setCell(CGL::Vector3D(x, y, z), cell{ black, AIR });
            }
        }
    }

    // floor 
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            setCell(CGL::Vector3D(x, 0, z), cell{ black, SAND });
        }
    }

    setCell(CGL::Vector3D(3, 3, 3), cell{black, SAND});
    setCell(CGL::Vector3D(6, 6, 6), cell{black, SAND});
    setCell(CGL::Vector3D(1, 3, 1), cell{black, SAND});
    setCell(CGL::Vector3D(1, 6, 1), cell{ black, SAND });

    
    for (int y = 7; y < CHUNK_SIZE; y++) {
        setCell(CGL::Vector3D(1, y, 1), cell{ black, SAND });
        setCell(CGL::Vector3D(2, y, 5), cell{ black, SAND });
        setCell(CGL::Vector3D(2, y, 6), cell{ black, SAND });
    }
    dirty_cells.reset();
}

void Chunk::update() {
    std::cout << "TICKING " << std::endl;
    // process 1 tick of time
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                //std::cout << getIndex(CGL::Vector3D(x, y, z)) << " ";
                if (dirty_cells.test(getIndex(CGL::Vector3D(x, y, z))) == false) {
                    type2func[getCell(CGL::Vector3D(x, y, z)).type](this, CGL::Vector3D(x, y, z));
                }
            }
        }
    }
    dirty_cells.reset();
    if (dirty_cells.test(256 == false)) {
        std::cout << "SUCCESSFUL RESET" << std::endl;
    }
}

cell& Chunk::getCell(CGL::Vector3D pos) {
    if (pos[0] < 0 || pos[0] >= CHUNK_SIZE ||
        pos[1] < 0 || pos[1] >= CHUNK_SIZE ||
        pos[2] < 0 || pos[2] >= CHUNK_SIZE) {
        return oob_cell;
    }

    return cells[getIndex(pos)];
}

void Chunk::setCell(CGL::Vector3D pos, cell cell) {
    cells[getIndex(pos)] = cell;
    dirty_cells.set(getIndex(pos));
    return;
}

void Chunk::swapCells(CGL::Vector3D curr_pos, CGL::Vector3D new_pos) {
    std::cout << new_pos[0] << " " << new_pos[1] << " " << new_pos[2];

    std::swap(getCell(curr_pos), getCell(new_pos));
    dirty_cells.set(getIndex(curr_pos));
    dirty_cells.set(getIndex(new_pos));
    return;
}         

void Chunk::simulate(int simulation_steps) {
    // not needed our system is realtime and smallest timestep is 1 frame, we are not "integrating" over timepoint
    // as in: we do not want to see sand jump 3 pixels per frame, sim steps is always 1 for us
    for (int i = 0; i < simulation_steps; i++) {
        if (i % 100 == 0) {
            setCell(CGL::Vector3D(3, 3, 3), cell{black, SAND});
        }
        update();
    }
}