#ifndef ENGINE_H
#define ENGINE_H

#include "PorterStemmer.h"
#include "InvertedIndex.h"

#include <boost/filesystem.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include <unordered_map>
#include "WildEngine.h"
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <list>

#include "KSerializer.h"
#include "KEngine.h"
#include "KgramIndex.h"
#include "DiskInvertedIndex.h"

class ClassifierEngine {
    boost::filesystem::path dir;

    InvertedIndex stemidx, idx; //Store the vocabulary, stem and unstem.
    QEngine queryEngine; //maybe use to classify

    std::unordered_map<uint32_t, std::string> idTable;

    //Need something to count number of term in class, not in...
    //class is author.
public:
    ClassifierEngine();



    //Extract the nuber of documents for each and put into method for the score.
    double featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass);

    /**
    * Takes a Porter Stemmer and a directory full of .json files to populate an inverted index that
    * is allocated on the heap. This method feeds the text file into a stringstream and passes the
    * stringstream into a boost, json property tree. The tokens are individually transformed to
    * lowercase and stemmed before being put into the inverted index.
    */
    void populateIndex(boost::filesystem::path &inDir, boost::filesystem::path &outDir);

    /**
    * This method goes to a path and walks through the directory searching for all files that end
    * with the .json extension. The method will put all full file paths into a vector that is passed
    * by reference.
    */
    void getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList);

    void printVocab();

    std::list<std::string> getVocab();

    void printIndex();

    void printQuery(std::string &query);

    std::vector<std::string> getQuery(std::string &query);

    std::string stem(std::string &token);
};

#endif
