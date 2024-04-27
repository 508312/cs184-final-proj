#include "world.h"
#include "chunk.h"
#include <iostream>


World::World() {
	createChunkIfDoesntExist(vec3(0, 0, 0));
}

World::~World() {
	for (auto entry = chunks.begin(); entry != chunks.end(); entry++) {
		delete entry->second;
	}

}

void World::setCell(vec3 pos, cell cell) {
	createChunkIfDoesntExist(pos);
	int chunk_index = getChunkIndex(pos);
	chunks[chunk_index]->setCell(getPosInChunk(pos), cell);
}

void World::spawnCell(vec3 pos, cell cell) {
	createChunkIfDoesntExist(pos);
	int chunk_index = getChunkIndex(pos);
	chunks[chunk_index]->spawnCell(getPosInChunk(pos), cell);
}

cell& World::getCell(vec3 pos) {
	createChunkIfDoesntExist(pos);
	int chunk_index = getChunkIndex(pos);

	//std::cout << "dance" << std::endl;
	cell& ret = chunks[chunk_index]->getCell(getPosInChunk(pos));
	//std::cout << "out" << std::endl;

	return ret;
}

void World::createChunkIfDoesntExist(vec3 pos) {
	int idx = getChunkIndex(pos);
	if (chunks.find(idx) == chunks.end()) {
		std::cout << "created " << idx << " at pos " << getChunkPos(pos)[0] << " " << getChunkPos(pos)[1] << " " << getChunkPos(pos)[2] << std::endl;
		chunks[idx] = new Chunk(this, getChunkPos(pos));
	}
}

// PRETTY SURE PASS BY COPY MAKES THIS REALLY SLOW FOR LARGE NUM CHUNKS
std::vector<Chunk*> World::getChunks() {
	std::vector<Chunk*> ret;
	int i = 0;
	for (auto entry = chunks.begin(); entry != chunks.end(); entry++) {
		ret.push_back(entry->second);
	}
	return ret;
}

// might be slow as well?
std::vector<Chunk*> World::update() {
	std::vector<Chunk*> updated_chunks;
	for (auto entry = chunks.begin(); entry != chunks.end(); entry++) {
		Chunk* chunk = entry->second;
		if (chunk->needsUpdate()) {
			chunk->update();
			updated_chunks.push_back(chunk);
		}
		
	}
	return updated_chunks;
}