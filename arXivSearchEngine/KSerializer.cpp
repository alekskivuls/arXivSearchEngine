#include "KSerializer.h"
#include <iomanip>

// For correcting endianness issues; you may not need these.
inline uint32_t Reverse(uint32_t value) {
    return (value & 0xFF000000) >> 24 |
        (value & 0x00FF0000) >> 8 |
        (value & 0x0000FF00) << 8 |
        (value & 0x000000FF) << 24;
}

inline uint64_t Reverse(uint64_t value) {
    return (value & 0xFF00000000000000) >> 56 |
        (value & 0x00FF000000000000) >> 40 |
        (value & 0x0000FF0000000000) >> 24 |
        (value & 0x000000FF00000000) >> 8 |
        (value & 0x00000000FF000000) << 8 |
        (value & 0x0000000000FF0000) << 24 |
        (value & 0x000000000000FF00) << 40 |
        (value & 0x00000000000000FF) << 56;
}

void WriteTerms(std::ofstream &kgramsFile, const std::list<std::string> &terms){
}

/*
 * This method is to build the file to store all of the kgrams to term pairings.
 * It works like an index and maps a token from Kgrams file made by buildKgrams
 * to a terms file made by buildTerms.
 */
static void buildIndex(const boost::filesystem::path &filePath, const KgramIndex &auxIdx){

}

// This method will build the file that stores all of the kgrams.
static std::vector<uint64_t> buildKgrams(const boost::filesystem::path &filePath, const KgramIndex &auxIdx){

}

// This method will build the file that stores all of the terms.
static void buildTerms(const boost::filesystem::path &filePath, const KgramIndex &auxIdx,
    const std::vector<uint64_t> &vocabPositions){

}
