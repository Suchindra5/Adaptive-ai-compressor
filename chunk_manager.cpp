#include <iostream>
#include "chunk_manager.h"

void ChunkManager::add_chunk(const char* buffer, size_t size){
    chunk.emplace_back(buffer, size);
}

size_t ChunkManager::get_chunk_count() const{
    return chunk.size();
}

const std::vector<Chunk>& ChunkManager::get_chunks() const{
    return chunk;
}