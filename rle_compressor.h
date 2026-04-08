#ifndef RLE_COMPRESSOR_H
#define RLE_COMPRESSOR_H

#include <vector>
#include <cstdint>

class RLECompressor {
public:
    RLECompressor() = default;

    // Compresses a 4KB chunk into an RLE bitstream
    std::vector<char> compress(const std::vector<char>& data);

    // Reconstructs the original 4KB chunk from RLE data
    std::vector<char> decompress(const std::vector<char>& data);
};

#endif