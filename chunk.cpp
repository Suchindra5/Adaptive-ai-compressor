#include <iostream>
#include "chunk.h"

Chunk::Chunk (const char *buffer, size_t size) : data(buffer, buffer+size){}

const std::vector<char>& Chunk::get_data() const{
    return data;
}

size_t Chunk::get_size() const{
    return data.size();
}