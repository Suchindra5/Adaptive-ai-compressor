#ifndef CHUNK_ANALYZER_H
#define CHUNK_ANALYZER_H

#include <vector>
#include <cstdint>

#define CHUNK_SIZE 4096
#define HEADER_SAMPLE_SIZE 4

struct ChunkFeatures {
    size_t size;
    double entropy;
    double variance;
    double repetition_ratio;
    double zero_byte_ratio;
    uint32_t magic_signature; // First 4 bytes for header detection
};

class ChunkAnalyzer {
public:
    // Main extraction routine
    static ChunkFeatures analyze(const std::vector<char>& data);

private:
    static double calculateEntropy(const std::vector<char>& data);
    static double calculateVariance(const std::vector<char>& data, double mean);
    static double calculateRepetitionRatio(const std::vector<char>& data);
    static uint32_t extractSignature(const std::vector<char>& data);
};

#endif