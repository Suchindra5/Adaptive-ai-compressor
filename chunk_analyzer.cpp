#include "chunk_analyzer.h"
#include <cmath>
#include <numeric>

ChunkFeatures ChunkAnalyzer::analyze(const std::vector<char>& data) {
    ChunkFeatures features{};
    features.size = data.size();

    if (data.empty()) return features;

    // 1. Extract Magic Signature (For the Decision Controller's first gate)
    features.magic_signature = extractSignature(data);

    // 2. Statistical Metrics
    // Calculate mean first to feed into variance
    double sum = 0.0;
    size_t zero_count = 0;
    for (char b : data) {
        unsigned char val = static_cast<unsigned char>(b);
        sum += val;
        if (val == 0) zero_count++;
    }
    double mean = sum / data.size();

    features.zero_byte_ratio = static_cast<double>(zero_count) / data.size();
    features.entropy = calculateEntropy(data);
    features.variance = calculateVariance(data, mean);
    features.repetition_ratio = calculateRepetitionRatio(data);

    return features;
}

double ChunkAnalyzer::calculateEntropy(const std::vector<char>& data) {
    uint32_t freq[256] = {0};
    for (char b : data) {
        freq[static_cast<unsigned char>(b)]++;
    }

    double entropy = 0.0;
    double data_size = static_cast<double>(data.size());
    for (uint32_t f : freq) {
        if (f > 0) {
            double p = f / data_size;
            // Standard check to ensure we don't log(0)
            if (p > 0) entropy -= p * std::log2(p);
        }
    }
    return entropy;
}

double ChunkAnalyzer::calculateVariance(const std::vector<char>& data, double mean) {
    double v_sum = 0.0;
    for (char b : data) {
        double diff = static_cast<unsigned char>(b) - mean;
        v_sum += diff * diff;
    }
    return v_sum / data.size();
}

double ChunkAnalyzer::calculateRepetitionRatio(const std::vector<char>& data) {
    if (data.size() < 2) return 0.0;
    size_t repeats = 0;
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] == data[i-1]) repeats++;
    }
    return static_cast<double>(repeats) / (data.size() - 1);
}

uint32_t ChunkAnalyzer::extractSignature(const std::vector<char>& data) {
    if (data.size() < HEADER_SAMPLE_SIZE) return 0;
    // Combine first 4 bytes into a single 32-bit integer for fast comparison
    uint32_t sig = 0;
    for (int i = 0; i < HEADER_SAMPLE_SIZE; ++i) {
        sig = (sig << 8) | static_cast<unsigned char>(data[i]);
    }
    return sig;
}