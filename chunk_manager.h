#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H
#include "chunk.h"
#include <vector>

class ChunkManager{
    public:
        void add_chunk(const char* buffer, size_t size);
        size_t get_chunk_count() const;
        const std::vector<Chunk>& get_chunks() const;

    private:
        std::vector<Chunk> chunk;
};

#endif