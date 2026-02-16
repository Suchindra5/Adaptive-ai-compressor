#include <iostream>
#include <string>
#include "file_reader.h"
#include "chunk.h"
#include "chunk_manager.h"

int main(){
    std::cout<<"Enter the file name: ";
    std::string file;
    std::cin>>file;

    FileReader reader(file);

    if (!reader.isOpen()) return 1;
    
    const size_t CHUNK_SIZE = 4096;
    char buffer[CHUNK_SIZE];

    ChunkManager manager;

    while (true){
        size_t bytes_read = reader.read(buffer, CHUNK_SIZE);

        if (bytes_read == 0) break;

        manager.add_chunk(buffer, bytes_read);
    }

    reader.close();
}