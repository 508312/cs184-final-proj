#include "chunk.h"
#include "cell_types.h"
#include "cell_funcs.h"
#include <iostream>
#include "cell.h"
#include <cassert>

#include "world.h"

color black = color{ 0, 0, 0, 0 };
cell oob_cell = cell{ black, WALL };

Chunk::Chunk(World* wp, vec3 chunk_index) {
    world = wp;
    chunk_pos = chunk_index;
    // air
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                spawnCell(vec3(x, y, z), cell{ black, AIR });
            }
        }
    }

    dirty_cells.reset();
}

void Chunk::update() {
    // process 1 tick of time
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (dirty_cells.test(getIndex(vec3(x, y, z))) == false) {
                    type2func[getCell(vec3(x, y, z)).type](this, vec3(x, y, z));
                }
            }
        }
    }
    dirty_cells.reset();
    if (dirty_cells.test(256 == false)) {
    }
}

cell& Chunk::getCell(vec3 pos) {
    if (pos[0] < 0 || pos[0] >= CHUNK_SIZE ||
        pos[1] < 0 || pos[1] >= CHUNK_SIZE ||
        pos[2] < 0 || pos[2] >= CHUNK_SIZE) {
        return world->getCell(pos + chunk_pos * CHUNK_SIZE);
    }

    return cells[getIndex(pos)];
}

void Chunk::spawnCell(vec3 pos, cell cell) {
    if (pos[0] < 0 || pos[0] >= CHUNK_SIZE ||
        pos[1] < 0 || pos[1] >= CHUNK_SIZE ||
        pos[2] < 0 || pos[2] >= CHUNK_SIZE) {
        return world->setCell(pos + chunk_pos * CHUNK_SIZE, cell);
    }

    cells[getIndex(pos)] = cell;
    return;
}

void Chunk::setCell(vec3 pos, cell cell) {
    if (pos[0] < 0 || pos[0] >= CHUNK_SIZE ||
        pos[1] < 0 || pos[1] >= CHUNK_SIZE ||
        pos[2] < 0 || pos[2] >= CHUNK_SIZE) {
        return world->setCell(pos + chunk_pos * CHUNK_SIZE, cell);
    }

    cells[getIndex(pos)] = cell;
    dirty_cells.set(getIndex(pos));
    return;
}

void Chunk::swapCells(vec3 curr_pos, vec3 new_pos) {
    cell a = getCell(curr_pos);
    cell b = getCell(new_pos);
    setCell(new_pos, a);
    setCell(curr_pos, b);
    //std::swap(getCell(curr_pos), getCell(new_pos));
    return;
}         

void Chunk::simulate(int simulation_steps) {
    // not needed our system is realtime and smallest timestep is 1 frame, we are not "integrating" over timepoint
    // as in: we do not want to see sand jump 3 pixels per frame, sim steps is always 1 for us
    for (int i = 0; i < simulation_steps; i++) {
        if (i % 100 == 0) {
            setCell(vec3(3, 3, 3), cell{black, SAND});
        }
        update();
    }
}