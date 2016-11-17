#ifndef KSERIALIZER_H
#define KSERIALIZER_H

#include "KgramIndex.h"
#include <boost/filesystem.hpp>
#include <cstdint>
#include <fstream>
#include <vector>
#include <string>


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
    static void buildIndex(boost::filesystem::path &filePath, KgramIndex &auxIdx1, KgramIndex &auxIdx2, KgramIndex &auxIdx3);

    // This method will build the file that stores all of the kgrams.
    static std::vector<uint32_t> buildKgrams(boost::filesystem::path &filePath, KgramIndex &auxIdx1, KgramIndex &auxIdx2, KgramIndex &auxIdx3);

    // This method will build the file that stores all of the terms.
    static void buildTerms(boost::filesystem::path &filePath, KgramIndex &auxIdx1, KgramIndex &auxIdx2, KgramIndex &auxIdx3,
        std::vector<uint32_t> &vocabPositions);

    static void WriteTerms(std::ofstream &kgramsFile, const std::unordered_set<std::string> &terms);
};

#endif
