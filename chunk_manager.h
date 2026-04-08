#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include "chunk_analyzer.h"
#include "decision_engine.h"
#include "rle_compressor.h"
#include "lz4_compressor.h"
#include "ans_compressor.h"

struct SessionStats {
    size_t total_chunks = 0;
    size_t rle_count = 0, lz4_count = 0, ans_count = 0, bypass_count = 0;
    size_t rle_savings = 0, lz4_savings = 0, ans_savings = 0;
    size_t original_total_size = 0;
    size_t compressed_total_size = 0;
    
    // Time tracking
    std::chrono::duration<double> total_time{0};
};

// Patent-Ready Struct: No padding for binary consistency
#pragma pack(push, 1)
struct ChunkHeader {
    uint8_t algorithm;
    size_t original_size;
    size_t compressed_size;
};
#pragma pack(pop)

class ChunkManager {
public:
    ChunkManager() = default;
    
    // Opens file and writes "HCE1" signature
    bool start_session(const std::string& output_path);
    void end_session();
    void print_stats();

    // The heart of the engine: Analyze -> Decide -> Compress -> Write
    void process_chunk(const char* buffer, size_t size);

private:
    std::ofstream outFile;
    ChunkAnalyzer analyzer;
    DecisionController controller;
    SessionStats stats;

    // Execution Units
    RLECompressor rle;
    LZ4Compressor lz4;
    ANSCompressor ans;
};

#endif