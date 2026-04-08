#ifndef DECISION_ENGINE_H
#define DECISION_ENGINE_H

#include "chunk_analyzer.h"
#include <string>

// Execution Path Enums - Represents physical hardware/software routing
enum CompressionPath {
    PATH_BYPASS,
    PATH_RLE,
    PATH_LZ4,
    PATH_ANS
};

// Result structure for the Pipeline Manager
struct ControlSignal {
    CompressionPath path;
    double complexity_index;
    const char* reason;
};

class DecisionController {
public:
    /**
     * @brief Evaluates chunk features and determines the optimal execution path.
     * This is the core 'Heuristic Engine' of the system.
     */
    static ControlSignal decide(const ChunkFeatures& features, size_t chunk_siz);

    /**
     * @brief Publicly accessible check for pre-compressed signatures.
     */
    static bool isKnownIncompressible(uint32_t sig);

private:
    // Proprietary constants for heuristic thresholds
    static constexpr double DCI_BYPASS_THRESHOLD = 7.4; 
    static constexpr double RLE_REPETITION_THRESHOLD = 0.65;
    static constexpr double SPARSE_ZERO_THRESHOLD = 0.50;
};

#endif