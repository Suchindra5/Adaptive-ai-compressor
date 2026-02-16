#include "file_reader.h"
#include <iostream>

FileReader::FileReader (const std::string& file){
    file_stream.open(file, std::ios::binary);
    if (!file_stream){
        std::cerr<<"Error opening the file"<<file<<std::endl;
    }
}

bool FileReader::isOpen() const{
    return file_stream.is_open();
}

size_t FileReader::read(char* buffer, size_t bytesToRead){
    if (!file_stream.is_open()) return 0;
    file_stream.read(buffer, bytesToRead);
    return static_cast<size_t>(file_stream.gcount());
}

void FileReader::close(){
    if(file_stream.is_open()) file_stream.close();
}
