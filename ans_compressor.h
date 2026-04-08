#ifndef ANS_COMPRESSOR_H
#define ANS_COMPRESSOR_H

#include <vector>
#include <cstdint>
#include <map>

class ANSCompressor {
public:
    ANSCompressor() = default;

    // Entropy-based encoding
    std::vector<char> compress(const std::vector<char>& data);

    // Statistical reconstruction
    std::vector<char> decompress(const std::vector<char>& data);

private:
    struct SymbolModel {
        uint16_t start;
        uint16_t freq;
    };
    const uint16_t TOTAL_RANGE = 4096; // Matches our CHUNK_SIZE
};

#endif