#include "chunk.h"
#include "cell_types.h"
#include "cell_funcs.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "cell.h"
#include <cassert>
#include <string>
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
    bbox_from = vec3(0, 0, 0);
    bbox_to = vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);

    dirty_cells.reset();
}

void Chunk::update() {
    // process 1 tick of time
    /*
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (dirty_cells.test(getIndex(vec3(x, y, z))) == false) {
                    type2func[getCell(vec3(x, y, z)).type](this, vec3(x, y, z));
                }
            }
        }
    }
    */

    // std::cout << "updating chunk " << getChunkPos().x << " " << getChunkPos().y << " " << getChunkPos().z
    //<< " from " << bbox_from.x << " " << bbox_from.y << " " << bbox_from.z
    //<< " to " << bbox_to.x << " " << bbox_to.y << " " << bbox_to.z << std::endl;

    vec3 bbox_from_copy = bbox_from;
    vec3 bbox_to_copy = bbox_to;
    resetBbox();

    for (int x = bbox_from_copy.x; x < bbox_to_copy.x; x++) {
        for (int y = bbox_from_copy.y; y < bbox_to_copy.y; y++) {
            for (int z = bbox_from_copy.z; z < bbox_to_copy.z; z++) {
                if (dirty_cells.test(getIndex(vec3(x, y, z))) == false) {
                    type2func[getCell(vec3(x, y, z)).type](this, vec3(x, y, z));
                } else {
                    vec3 d = vec3(x, y, z);
                    expandBbox(d);
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
    expandBbox(pos);
    return;
}

void Chunk::resetBbox() {
    bbox_to = vec3(-1, -1, -1);
    bbox_from = vec3(CHUNK_SIZE + 1, CHUNK_SIZE + 1, CHUNK_SIZE + 1);
}

void Chunk::expandBbox(vec3& pos) {
    /*
    if (pos.x == 0) {
        world
    } else if (pos.x == CHUNK_SIZE) {

    }
    if (pos.y == 0) {

    } else if (pos.y == CHUNK_SIZE) {

    }
    if (pos.z == 0) {

    } else if (pos.z == CHUNK_SIZE) {

    } */

    bbox_to.x = std::max(bbox_to.x, std::min(pos.x + 2, CHUNK_SIZE));
    bbox_to.y = std::max(bbox_to.y, std::min(pos.y + 2, CHUNK_SIZE));
    bbox_to.z = std::max(bbox_to.z, std::min(pos.z + 2, CHUNK_SIZE));
    bbox_from.x = std::min(bbox_from.x, std::max(pos.x - 1, 0));
    bbox_from.y = std::min(bbox_from.y, std::max(pos.y - 1, 0));
    bbox_from.z = std::min(bbox_from.z, std::max(pos.z - 1, 0));
}

void Chunk::setCell(vec3 pos, cell cell) {
    if (pos[0] < 0 || pos[0] >= CHUNK_SIZE ||
        pos[1] < 0 || pos[1] >= CHUNK_SIZE ||
        pos[2] < 0 || pos[2] >= CHUNK_SIZE) {
        return world->setCell(pos + chunk_pos * CHUNK_SIZE, cell);
    }

    cells[getIndex(pos)] = cell;
    dirty_cells.set(getIndex(pos));
    expandBbox(pos);
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

void Chunk::dumpChunk(std::ofstream& file) {
    //std::cout << " dumping " << chunk_pos.x << " " << chunk_pos.y << " " << chunk_pos.z << std::endl;
    file << chunk_pos.x << " " << chunk_pos.y << " " << chunk_pos.z << std::endl;
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
        file << cells[i] << " ";
    }
}

void Chunk::loadChunk(std::ifstream& file) {
    //file >> chunk_pos.x >> chunk_pos.y >> chunk_pos.z;
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
        file >> cells[i];
    }
}

void Chunk::resetDirty() {
    dirty_cells.reset();
}