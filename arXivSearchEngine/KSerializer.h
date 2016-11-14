#ifndef KSERIALIZER_H
#define KSERIALIZER_H

#include "KgramIndex.h"
#include <boost/filesystem.hpp>
#include <cstdint>
#include <fstream>
#include <vector>

// does not need to be an object. This should be a utility class
// that is resposible for serializing and nothing else.

/**
 * This class serializes the KgramIndex; writing it to disk.
 */
class KSerializer {
public:

    // This method is to build the file to store all of the kgrams to term pairings.
    // It works like an index and maps a token from Kgrams file made by buildKgrams
    // to a terms file made by buildTerms.
    static void buildIndex(const boost::filesystem::path &filePath, const KgramIndex &auxIdx);

    // This method will build the file that stores all of the kgrams.
    static std::vector<uint64_t> buildKgrams(const boost::filesystem::path &filePath, const KgramIndex &auxIdx);

    // This method will build the file that stores all of the terms.
    static void buildTerms(const boost::filesystem::path &filePath, const KgramIndex &auxIdx,
        const std::vector<uint64_t> &vocabPositions);

    static void WriteTerms(std::ofstream &kgramsFile, const std::list<std::string> &terms);
};

#endif
