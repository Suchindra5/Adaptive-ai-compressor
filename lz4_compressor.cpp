#include "lz4_compressor.h"
#include <vector>
#include <cstdint>
#include <cstring>

// --- COMPRESSOR ---
std::vector<char> LZ4Compressor::compress(const std::vector<char>& data) {
    std::vector<char> compressed;
    int dataSize = static_cast<int>(data.size());
    
    // Hash table to store the last seen position of 4-byte sequences
    // 4096 entries (12-bit hash) keeps it small and fast
    std::vector<int> hashTable(4096, -1);

    for (int i = 0; i < dataSize; ) {
        int matchLen = 0;
        int matchDist = 0;

        // 1. Only look for matches if we have enough data left
        if (i + 4 < dataSize) {
            // Simple Hash: Combine 4 bytes into one index
            uint32_t sequence = *reinterpret_cast<const uint32_t*>(&data[i]);
            uint32_t hash = ((sequence >> 12) ^ sequence) & 0xFFF; // 12-bit mask

            int prevPos = hashTable[hash];
            hashTable[hash] = i; // Update table with current position

            // 2. Check if the hash hit is a real match and within 4KB
            if (prevPos != -1 && (i - prevPos) < 4096) {
                while (matchLen < 255 && (i + matchLen + 1) < dataSize && 
                       data[prevPos + matchLen] == data[i + matchLen]) {
                    matchLen++;
                }
                matchDist = i - prevPos;
            }
        }

        // 3. Write LZ4-style Token: [Dist High, Dist Low, Length, Literal]
        // This format matches your current decompressor!
        compressed.push_back(static_cast<char>((matchDist >> 8) & 0xFF));
        compressed.push_back(static_cast<char>(matchDist & 0xFF));
        compressed.push_back(static_cast<char>(matchLen & 0xFF));
        
        // Advance i to the character after the match
        i += matchLen;
        
        // Add the Literal character
        if (i < dataSize) {
            compressed.push_back(data[i]);
            i++;
        } else {
            compressed.push_back('\0');
        }
    }
    return compressed;
}

// --- DECOMPRESSOR ---
std::vector<char> LZ4Compressor::decompress(const std::vector<char>& compressed, size_t expected_size) {
    std::vector<char> decompressed;
    decompressed.reserve(expected_size);

    for (size_t i = 0; i + 3 < compressed.size(); i += 4) {
        int dist = (static_cast<unsigned char>(compressed[i]) << 8) | 
                    static_cast<unsigned char>(compressed[i + 1]);
        int len = static_cast<unsigned char>(compressed[i + 2]);
        char literal = compressed[i + 3];

        // 1. Copy the match
        if (len > 0 && dist > 0 && dist <= static_cast<int>(decompressed.size())) {
            int start = static_cast<int>(decompressed.size()) - dist;
            for (int j = 0; j < len; ++j) {
                decompressed.push_back(decompressed[start + j]);
            }
        }

        // 2. Add the literal
        if (decompressed.size() < expected_size) {
            decompressed.push_back(literal);
        }
    }
    return decompressed;
}