#ifndef LZ4_COMPRESSOR_H
#define LZ4_COMPRESSOR_H

#include <vector>
#include <cstddef> // For size_t

class LZ4Compressor {
public:
    /**
     * @brief Compresses data using a High-Speed Hash-based LZ4 approach.
     * @param data The raw input buffer.
     * @return A vector containing 4-byte tokens: [DistHigh, DistLow, Length, Literal].
     */
    std::vector<char> compress(const std::vector<char>& data);

    /**
     * @brief Decompresses the 4-byte token stream back into original data.
     * @param compressed The compressed token buffer.
     * @param expected_size The original size of the chunk (from the header).
     * @return The reconstructed raw data.
     */
    std::vector<char> decompress(const std::vector<char>& compressed, size_t expected_size);

private:
    // Constants for the sliding window and hash table
    static const int WINDOW_SIZE = 4096;
    static const int HASH_TABLE_SIZE = 4096;
};

#endif