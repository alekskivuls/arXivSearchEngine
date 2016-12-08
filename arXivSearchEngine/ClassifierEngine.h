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
#include <queue>

#include "KSerializer.h"
#include "KEngine.h"
#include "KgramIndex.h"
#include "DiskInvertedIndex.h"

class ClassifierEngine {
    boost::filesystem::path dir;

    DiskInvertedIndex _idx; //Store the vocabulary, stem and unstem.
    QEngine queryEngine; //maybe use to classify and merges.

    //Greatest double first.
    std::priority_queue<std::pair<double, std::string>> madison, jay, hamilton, globalclass;

    //Need something to count number of term in class, not in...
    //class is author.classTerm
public:
    ClassifierEngine();
    //ClassifierEngine(const DiskInvertedIndex &idx);
    //Extract the nuber of documents for each and put into method for the score.
    double featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass);

    double countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    double countClass(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    double countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    double countNone(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);

    void driver();

    //Get the all time top rank.
    std::list<std::string> getGlobalTop(uint32_t n);
    //Access the Hamilton, Jay or Madison Classes
    std::list<std::string> getTopClass(std::string author, uint32_t n);

    //Set the DiskInvertedIndex
    void setIndex(DiskInvertedIndex &idx);
};

#endif
