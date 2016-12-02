#ifndef KDESERIALIZER_H
#define KDESERIALIZER_H

#include "KgramIndex.h"
#include <boost/filesystem.hpp>
#include <cstdint>
#include <fstream>
#include <vector>
#include <string>
#include <list>


// does not need to be an object. This should be a utility class
// that is resposible for serializing and nothing else.


struct KgramEntry {
    uint32_t KgramPosition;
    uint32_t TermPosition;

    KgramEntry() : KgramPosition(0), TermPosition(0) {}
    KgramEntry(uint32_t kgram, uint32_t term) : KgramPosition(kgram), TermPosition(term) {}
};

/**
 * This class serializes the KgramIndex; writing it to disk.
 */
class KDeserializer {
public:
    static std::string ReadTerm(std::ifstream &terms, uint32_t lastTermloc); // implement this after i fix the read method...
    //may not even need above method.

    static uint32_t ReadInt(std::ifstream &stream);

    boost::filesystem::path mPath;
    mutable std::ifstream mKgramList;
    mutable std::ifstream mTerms;
    std::vector<KgramEntry> mKgramTable; //This is where the kgramtable.bin is translated into.

    //convert terms from file into list <string>
    std::unordered_set<std::string> ReadTermsFromFile(std::ifstream &terms, uint32_t termsPosition); // std::vector<DocInfo>
    //where to start reading terms
    static std::vector<KgramEntry> ReadKgramTable(const boost::filesystem::path &path);

    KgramEntry BinarySearchKgrams(const std::string &kgram) const;


    std::string ReadKgramStringAtPosition(uint32_t index) const;

    //constructor just needs the location of the files.
    KDeserializer(const boost::filesystem::path  &path);

    std::unordered_set<std::string> GetTerms(std::string &kgram); // std::vector<DocInfo>

    void printAllTerms(KgramIndex &idx);

    void toKgramIndex(KgramIndex &idx1, KgramIndex &idx2, KgramIndex &idx3);
};

#endif
