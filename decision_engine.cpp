#include "decision_engine.h"

ControlSignal DecisionController::decide(const ChunkFeatures& features, size_t chunk_size) {
    ControlSignal signal;
    // Calculate DCI once for use in logic and reporting
    signal.complexity_index = features.entropy / (1.0 + features.repetition_ratio);

    // GATE 1: Known Signatures (O(1) lookup)
    if (isKnownIncompressible(features.magic_signature)) {
        signal.path = PATH_BYPASS;
        signal.reason = "PRECOMPRESSED_FORMAT";
        return signal;
    }

    if (chunk_size < 512) { 
        // If the chunk is tiny (like your 268-byte tail), 
        // don't even try to compress it. Just bypass.
        signal.path = PATH_BYPASS;
        signal.reason = "CHUNK_TOO_SMALL_FOR_ALGO";
        return signal;
    }

    // GATE 2: Sparse/Empty Data (High zero count)
    if (features.zero_byte_ratio > SPARSE_ZERO_THRESHOLD) {
        signal.path = PATH_RLE;
        signal.reason = "SPARSE_ZERO_DATA";
        return signal;
    }

    // GATE 3: Multi-Modal Intelligent Routing
    if (signal.complexity_index > DCI_BYPASS_THRESHOLD) {
        signal.path = PATH_BYPASS;
        signal.reason = "HIGH_ENTROPY_NOISE";
    } 
    else if (features.repetition_ratio > RLE_REPETITION_THRESHOLD) {
        signal.path = PATH_RLE;
        signal.reason = "REPETITIVE_RUNS";
    }
    // ANS Trigger: Skewed byte frequency (High Variance)
    else if (features.entropy > 7.5 && features.variance > 3000.0) {
        signal.path = PATH_ANS;
        signal.reason = "SKEWED_STATISTICS_ANS";
    }
    else {
        // Default to LZ4 for structured data
        signal.path = PATH_LZ4;
        signal.reason = "STRUCTURAL_PATTERNS_LZ4";
    }

    return signal;
}

bool DecisionController::isKnownIncompressible(uint32_t sig) {
    switch (sig) {
        case 0x504B0304: // ZIP / DOCX / PPTX
        case 0xFFD8FFE0: // JPEG
        case 0x89504E47: // PNG
        case 0x25504446: // PDF (%PDF)
        case 0x28B52FFD: // Zstandard
        case 0x1F8B0800: // GZIP
        case 0x47494638: // GIF
            return true;
        default:
            return false;
    }
}