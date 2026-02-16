#ifndef CHUNK_H
#define CHUNK_H

#include <iostream>
#include <vector>

class Chunk{
    public:
        Chunk(const char *buffer, size_t size);
        const std::vector<char>& get_data() const;
        size_t get_size() const;

    private:
        std::vector<char> data;
};

#endif