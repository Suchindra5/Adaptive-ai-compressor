#ifndef FILE_READER_H
#define FILE_READER_H

#include <string>
#include <fstream>

class FileReader{
    public:
        explicit FileReader(const std::string& file);
        bool isOpen() const;
        size_t read(char* buffer, size_t bytesToRead);
        void close();

    private:
        std::ifstream file_stream;
};

#endif