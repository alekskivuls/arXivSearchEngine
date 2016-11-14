#include "KDeserializer.h"
#include <iomanip>
#include <iostream>

using std::ifstream;
namespace fs = boost::filesystem;

extern uint32_t Reverse(uint32_t);
//extern uint64_t Reverse(uint64_t);
//extern double_t Reverse(double_t);


//uint32_t KDeserializer::ReadInt(std::ifstream &stream) { //here
//    uint32_t value = 0;
//    stream.read((char*)&value, sizeof(value));
//    return Reverse(value); // UNCOMMENT FOR WINDDOWS
//}

////constructor just needs the location of the files.
//KDeserializer::KDeserializer(const boost::filesystem::path  &path) : mPath(path) {
//    mVocabList.open(boost::filesystem::path(mPath).append("/vocabList.bin", boost::filesystem::path::codecvt()).string(),
//        std::ios_base::in | std::ios_base::binary);
//    mPostings.open(boost::filesystem::path(mPath).append("/postings.bin", boost::filesystem::path::codecvt()).string(),
//        std::ios_base::in | std::ios_base::binary);

//    // open the vocabulary table and read it into memory.
//    // we will end up with an array of T pairs of longs, where the first value is
//    // a position in the vocabularyTable file, and the second is a position in
//    // the postings file.
//    mVocabTable = ReadVocabTable(path);
//}


//boost::filesystem::path mPath;
//mutable std::ifstream mKgramList;
//mutable std::ifstream mTerms;
//std::vector<KgramEntry> mKgramTable;

////where to start reading terms
//std::vector<KgramEntry> KDeserializer::ReadKgramTable(const boost::filesystem::path &path){
//    fs::path tablePath = path;

//    ifstream tableFile(tablePath.append("/vocabTable.bin", boost::filesystem::path::codecvt()).string(),
//        std::ios::in | std::ios::binary);
//    uint32_t buffer, buffer2;

//    tableFile.read((char *)&buffer, sizeof(buffer));
//    buffer = Reverse(buffer);// UNCOMMENT FOR LINUX

//    int tableIndex = 0;

//    std::vector<VocabEntry> vocabTable;
//    vocabTable.reserve(buffer);

//    while (tableFile.read((char*)&buffer, sizeof(buffer))) {
//        tableFile.read((char*)&buffer2, sizeof(buffer2));

//        // Again, you may not need the reverse calls.
//        vocabTable.emplace_back(Reverse(buffer), Reverse(buffer2));// UNCOMMENT FOR LINUX
//    }

//    std::cout << "KLAJSD SIZE OF VOCAB TABLEEEE HUUUUUUUU! " << vocabTable.size() << std::endl;

//    return vocabTable;
//}

////convert terms from file into list <string>
//std::list<std::string> KDeserializer::ReadTermsFromFile(std::ifstream &terms, uint32_t termsPosition); // std::vector<DocInfo>


//KgramEntry KDeserializer::BinarySearchKgrams(const std::string &kgram) const;
//std::string KDeserializer::ReadTerm(std::ifstream &terms, uint32_t lastTermloc); // implement this after i fix the read method...
////may not even need above method.

//std::string KDeserializer::ReadKgramStringAtPosition(uint32_t index) const;


//std::list<std::string> KDeserializer::GetTerms(const std::string &kgram) const; // std::vector<DocInfo>

//void KDeserializer::printAllTerms(const KgramIndex &idx);
