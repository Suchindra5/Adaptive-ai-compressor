#include "rle_compressor.h"

std::vector<char> RLECompressor::compress(const std::vector<char>& data) {
    std::vector<char> compressed;
    if (data.empty()) return compressed;

    // Pre-allocate memory to avoid multiple reallocations
    compressed.reserve(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        uint8_t count = 1;
        char current_byte = data[i];

        // Count consecutive occurrences, maxing out at 255 (size of uint8_t)
        while (i + 1 < data.size() && data[i + 1] == current_byte && count < 255) {
            count++;
            i++;
        }

        // Store as a pair: [1-byte count] [1-byte value]
        compressed.push_back(static_cast<char>(count));
        compressed.push_back(current_byte);
    }

    return compressed;
}

std::vector<char> RLECompressor::decompress(const std::vector<char>& data) {
    std::vector<char> out;
    // Guard: RLE data must be in pairs
    if (data.size() % 2 != 0) return out; 

    for (size_t i = 0; i + 1 < data.size(); i += 2) {
        uint8_t count = static_cast<uint8_t>(data[i]);
        char value = data[i + 1];
        out.insert(out.end(), count, value);
    }
    return out;
}