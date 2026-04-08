#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include "file_reader.h"
#include "chunk_manager.h"

#define CHUNK_SIZE 4096

#include <filesystem>
namespace fs = std::filesystem;

void compress();
void decompress();

int main() {
    while (true) {
        std::cout << "\n1. Compress\n2. Decompress\n3. Exit\nChoice: ";
        int choice;
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            compress();
            break;
        case 2:
            decompress();
            break;
        case 3:
            std::cout<<"Exiting....";
            return 0;
            break;
        default:
            std::cout<<"Invalid Choice..";
        }

        if (choice == 1) compress();
        else if (choice == 2) decompress();
        else if (choice == 3) break;
        else std::cout << "Invalid choice.\n";
    }
    return 0;
}

void compress() {
    std::cout << "Enter source file: ";
    std::string file;
    std::cin >> file;

    FileReader reader(file);
    if (!reader.isOpen()) {
        std::cerr << "Error: Could not open " << file << "\n";
        return;
    }

    ChunkManager manager;
    // We append .hce to the output filename
    if (manager.start_session(file + ".hce")) {
        char buffer[CHUNK_SIZE];
        
        while (true) {
            size_t bytes_read = reader.read(buffer, CHUNK_SIZE);
            
            // 1. If we read data, process it (even if it's less than 4096)
            if (bytes_read > 0) {
                manager.process_chunk(buffer, bytes_read);
            }

            // 2. Check if we've reached the end of the file
            // If bytes_read is less than CHUNK_SIZE, there's no more data left.
            if (bytes_read < CHUNK_SIZE) {
                break;
            }
        }
        
        manager.end_session();
        manager.print_stats(); // Display the final report
        std::cout << "\nCompression successful: " << file << ".hce\n";
    }
    reader.close();
}


void decompress() {
    std::cout << "Enter .hce file to decompress: ";
    std::string input_path;
    std::cin >> input_path;

    // 1. Find the last slash to get JUST the filename
    size_t last_slash = input_path.find_last_of("\\/");
    std::string folder_path = "";
    std::string filename = input_path;

    if (last_slash != std::string::npos) {
        folder_path = input_path.substr(0, last_slash + 1);
        filename = input_path.substr(last_slash + 1);
    }

    // 2. Remove .hce extension if it exists
    size_t ext_pos = filename.find(".hce");
    std::string clean_name = (ext_pos != std::string::npos) ? filename.substr(0, ext_pos) : filename;

    // 3. Combine them: Folder + "decompressed_" + Filename
    std::string output_file = folder_path + "decompressed_" + clean_name;

    std::ifstream in(input_path, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "Error: Could not open INPUT file at: " << input_path << "\n";
        return;
    }
    if (!out.is_open()) {
        std::cerr << "Error: Could not create OUTPUT file at: " << output_file << "\n";
        std::cerr << "Hint: Check if the folder is read-only or the path is invalid.\n";
        return;
    }

    // 2. Verify Magic Signature (4 bytes)
    char magic[4];
    in.read(magic, 4);
    if (std::string(magic, 3) != "HCE") {
        std::cerr << "Error: Invalid HCE signature.\n";
        return;
    }

    // Initialize algorithms
    RLECompressor rle; 
    LZ4Compressor lz4;
    ANSCompressor ans;

    ChunkHeader header;
    int chunkCount = 0;

    // 3. Main Loop: Read the header FIRST
    while (in.read(reinterpret_cast<char*>(&header), sizeof(ChunkHeader))) {
        
        // Safety: Ensure we don't try to allocate zero or impossible memory
        if (header.compressed_size == 0) continue;

        std::vector<char> compressed_buffer(header.compressed_size);
        
        // 4. Read exactly the amount specified by the header
        if (!in.read(compressed_buffer.data(), header.compressed_size)) {
            std::cerr << "Error: Unexpected end of file at chunk " << chunkCount << "\n";
            break;
        }

        std::vector<char> decompressed_data;

        // 5. Algorithm Selection based on Header ID
        switch (header.algorithm) {
            case PATH_RLE: // PATH_RLE
                decompressed_data = rle.decompress(compressed_buffer);
                break;
            case PATH_LZ4: // PATH_LZ77
                decompressed_data = lz4.decompress(compressed_buffer, header.original_size);;
                break;
            case PATH_ANS: // PATH_ANS
                decompressed_data = ans.decompress(compressed_buffer);
                break;
            default: // PATH_BYPASS
                decompressed_data = compressed_buffer;
                break;
        }

        // 6. Hard-Constraint: Ensure the output matches the original chunk size
        if (decompressed_data.size() > header.original_size) {
            decompressed_data.resize(header.original_size);
        }

        if (!decompressed_data.empty()) {
            out.write(decompressed_data.data(), decompressed_data.size());
        }
        chunkCount++;
    }

    // 7. Cleanup
    out.flush();
    out.close();
    in.close();

    std::cout << "\nDecompression finished. Processed " << chunkCount << " chunks." << std::endl;
}