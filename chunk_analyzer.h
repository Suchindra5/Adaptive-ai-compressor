#ifndef CHUNK_ANALYZER_H
#define CHUNK_ANALYZER_H

class Analyzer{
    public:
        void entropy();
        void variance();
        void repititon_ratio();
        void zero_byte_ratio();
    private:
        typedef struct Chunk_Analyzer
        {
            double entropy, variance, repitition_ratio, zero_byte_ratio;
        }chunk_analyzer;
        
};

#endif