#include "chunk_manager.h"
#include <iostream>
#include <vector>

bool ChunkManager::start_session(const std::string& output_path) {
    outFile.open(output_path, std::ios::binary);
    if (!outFile.is_open()) return false;

    // Reset Stats for a new file
    stats = SessionStats();

    // Write Magic Signature
    const char MAGIC[4] = {'H', 'C', 'E', 0x01};
    outFile.write(MAGIC, 4);
    return true;
}

void ChunkManager::process_chunk(const char* buffer, size_t size) {
    if (size == 0) return;
    
    auto start = std::chrono::high_resolution_clock::now();

    // 1. Wrap raw pointer for internal processing
    std::vector<char> data(buffer, buffer + size);

    // 2. Heuristic Logic
    ChunkFeatures features = analyzer.analyze(data);

    ControlSignal signal = controller.decide(features, size);

    std::vector<char> outData; // Declared once here
    long current_savings = 0;

    if (size < 512) {
        signal.path = PATH_BYPASS;
    }

    // 3. Execution & Specific Savings Tracking
    switch (signal.path) {
        case PATH_RLE:  
            outData = rle.compress(data); 
            stats.rle_count++;
            current_savings = static_cast<long>(size) - outData.size();
            stats.rle_savings += (current_savings > 0 ? current_savings : 0);
            break;
            
        case PATH_LZ4: 
            outData = lz4.compress(data); 
            stats.lz4_count++;
            current_savings = static_cast<long>(size) - outData.size();
            stats.lz4_savings += (current_savings > 0 ? current_savings : 0);
            break;
            
        case PATH_ANS:  
            outData = ans.compress(data); 
            // If ANS failed to write a table or actually grew the data, FIX IT HERE
            if (outData.empty() || outData.size() >= size) {
                outData = data;
                signal.path = PATH_BYPASS; // Change the ID for the header
                break;
            }
            stats.ans_count++;
            current_savings = static_cast<long>(size) - outData.size();
            stats.ans_savings += (current_savings > 0 ? current_savings : 0);
            break;
            
        default: // PATH_BYPASS
            outData = data; 
            stats.bypass_count++; 
            break; 
    }

    // 4. Construct and Write Header
    ChunkHeader header;
    header.algorithm = static_cast<uint8_t>(signal.path);
    header.original_size = static_cast<uint32_t>(size);
    header.compressed_size = static_cast<uint32_t>(outData.size());

    outFile.write(reinterpret_cast<const char*>(&header), sizeof(ChunkHeader));
    
    // 5. Write Payload
    outFile.write(outData.data(), outData.size());

    // 6. Finalize General Stats
    auto end = std::chrono::high_resolution_clock::now();
    stats.total_time += (end - start);
    stats.total_chunks++;
    stats.original_total_size += size;
    stats.compressed_total_size += outData.size();
}

void ChunkManager::end_session() {
    if (outFile.is_open()) outFile.close();
}

void ChunkManager::print_stats() {
    double total_sec = stats.total_time.count();
    // Prevent division by zero if file is tiny/empty
    if (total_sec == 0) total_sec = 0.0001; 

    double mb_per_sec = (stats.original_total_size / 1024.0 / 1024.0) / total_sec;
    double ratio = (static_cast<double>(stats.compressed_total_size) / stats.original_total_size) * 100.0;

    std::cout << "\n==================================================";
    std::cout << "\n           HCE COMPRESSION REPORT             ";
    std::cout << "\n==================================================";
    std::cout << "\n[GENERAL METRICS]";
    std::cout << "\nTotal Time:         " << total_sec << " seconds";
    std::cout << "\nThroughput:         " << mb_per_sec << " MB/s";
    std::cout << "\nCompression Ratio:  " << ratio << "%";
    
    std::cout << "\n\n[ALGORITHM SELECTION]";
    std::cout << "\nBypass: " << stats.bypass_count << " | RLE: " << stats.rle_count 
              << " | LZ4: " << stats.lz4_count << " | ANS: " << stats.ans_count;

    std::cout << "\n\n[SPACE SAVINGS]";
    std::cout << "\nRLE Saved:   " << stats.rle_savings / 1024.0 << " KB";
    std::cout << "\nLZ4 Saved:  " << stats.lz4_savings / 1024.0 << " KB";
    std::cout << "\nANS Saved:   " << stats.ans_savings / 1024.0 << " KB";
    
    std::cout << "\n\n[FINAL RESULTS]";
    std::cout << "\nOriginal:    " << stats.original_total_size / 1024.0 << " KB";
    std::cout << "\nCompressed:  " << stats.compressed_total_size / 1024.0 << " KB";
    
    long total_saved = static_cast<long>(stats.original_total_size) - static_cast<long>(stats.compressed_total_size);
    std::cout << "\nTotal Saved: " << total_saved / 1024.0 << " KB";
    std::cout << "\n==================================================\n";
}